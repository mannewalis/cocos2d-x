/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "AudioEngine-inl.h"

#include <unordered_map>
// for native asset manager
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "audio/include/AudioEngine.h"
#include "base/CCDirector.h"
#include "platform/android/CCFileUtils-android.h"

#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#include <jni.h>

using namespace cocos2d;
using namespace cocos2d::experimental;

void PlayOverEvent(SLPlayItf caller, void* context, SLuint32 playEvent)
{
    if (context && playEvent == SL_PLAYEVENT_HEADATEND)
    {
        AudioEngineImpl* engineImpl = (AudioEngineImpl*)context;
        engineImpl->playerFinishCallback(caller,playEvent);
    }
}

AudioPlayer::AudioPlayer()
    : _fdPlayerObject(nullptr)
    , _finishCallback(nullptr)
    , _duration(0.0f)
{

}

AudioPlayer::~AudioPlayer()
{
    if (_fdPlayerObject)
    {
        (*_fdPlayerObject)->Destroy(_fdPlayerObject);
        _fdPlayerObject = nullptr;
    }
}

bool AudioPlayer::init(SLEngineItf engineEngine, SLObjectItf outputMixObject,const std::string& fileFullPath, float volume, bool loop)
{
    bool ret = false;

    do 
    {
        SLDataSource audioSrc;

        SLDataLocator_AndroidFD loc_fd;
        SLDataLocator_URI loc_uri;

        SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
        audioSrc.pFormat = &format_mime;

        if (fileFullPath[0] != '/'){
            std::string relativePath = "";

            size_t position = fileFullPath.find("assets/");
            if (0 == position) {
                // "assets/" is at the beginning of the path and we don't want it
                relativePath += fileFullPath.substr(strlen("assets/"));
            } else {
                relativePath += fileFullPath;
            }

            auto asset = AAssetManager_open(cocos2d::FileUtilsAndroid::getAssetManager(), relativePath.c_str(), AASSET_MODE_UNKNOWN);

            // open asset as file descriptor
            off_t start, length;
            int fd = AAsset_openFileDescriptor(asset, &start, &length);
            if (fd <= 0){
                AAsset_close(asset);
                break;
            }
            AAsset_close(asset);

            // configure audio source
            loc_fd = {SL_DATALOCATOR_ANDROIDFD, fd, start, length};

            audioSrc.pLocator = &loc_fd;
        }
        else{
            loc_uri = {SL_DATALOCATOR_URI , (SLchar*)fileFullPath.c_str()};
            audioSrc.pLocator = &loc_uri;
        }

        // configure audio sink
        SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
        SLDataSink audioSnk = {&loc_outmix, NULL};

        // create audio player
        const SLInterfaceID ids[3] = {SL_IID_SEEK, SL_IID_PREFETCHSTATUS, SL_IID_VOLUME};
        const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
        auto result = (*engineEngine)->CreateAudioPlayer(engineEngine, &_fdPlayerObject, &audioSrc, &audioSnk, 3, ids, req);
        if(SL_RESULT_SUCCESS != result){ ERRORLOG("create audio player fail"); break; }

        // realize the player
        result = (*_fdPlayerObject)->Realize(_fdPlayerObject, SL_BOOLEAN_FALSE);
        if(SL_RESULT_SUCCESS != result){ ERRORLOG("realize the player fail"); break; }

        // get the play interface
        result = (*_fdPlayerObject)->GetInterface(_fdPlayerObject, SL_IID_PLAY, &_fdPlayerPlay);
        if(SL_RESULT_SUCCESS != result){ ERRORLOG("get the play interface fail"); break; }

        // get the seek interface
        result = (*_fdPlayerObject)->GetInterface(_fdPlayerObject, SL_IID_SEEK, &_fdPlayerSeek);
        if(SL_RESULT_SUCCESS != result){ ERRORLOG("get the seek interface fail"); break; }

        // get the volume interface
        result = (*_fdPlayerObject)->GetInterface(_fdPlayerObject, SL_IID_VOLUME, &_fdPlayerVolume);
        if(SL_RESULT_SUCCESS != result){ ERRORLOG("get the volume interface fail"); break; }

        if (loop){
            (*_fdPlayerSeek)->SetLoop(_fdPlayerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
        }

        int dbVolume = 2000 * log10(volume);
        if(dbVolume < SL_MILLIBEL_MIN){
            dbVolume = SL_MILLIBEL_MIN;
        }
        (*_fdPlayerVolume)->SetVolumeLevel(_fdPlayerVolume, dbVolume);

        result = (*_fdPlayerPlay)->SetPlayState(_fdPlayerPlay, SL_PLAYSTATE_PLAYING);
        if(SL_RESULT_SUCCESS != result){ ERRORLOG("SetPlayState fail"); break; }

        ret = true;
    } while (0);

    return ret;
}

//====================================================
AudioEngineImpl::AudioEngineImpl()
    : currentAudioID(0)
    , _engineObject(nullptr)
    , _engineEngine(nullptr)
    , _outputMixObject(nullptr)
{

}

AudioEngineImpl::~AudioEngineImpl()
{
    if (_outputMixObject)
    {
        (*_outputMixObject)->Destroy(_outputMixObject);
    }
    if (_engineObject)
    {
        (*_engineObject)->Destroy(_engineObject);
    }
}

bool AudioEngineImpl::init()
{
    bool ret = false;
    do{
        // create engine
        auto result = slCreateEngine(&_engineObject, 0, nullptr, 0, nullptr, nullptr);
        if(SL_RESULT_SUCCESS != result){ ERRORLOG("create opensl engine fail"); break; }

        // realize the engine
        result = (*_engineObject)->Realize(_engineObject, SL_BOOLEAN_FALSE);
        if(SL_RESULT_SUCCESS != result){ ERRORLOG("realize the engine fail"); break; }

        // get the engine interface, which is needed in order to create other objects
        result = (*_engineObject)->GetInterface(_engineObject, SL_IID_ENGINE, &_engineEngine);
        if(SL_RESULT_SUCCESS != result){ ERRORLOG("get the engine interface fail"); break; }

        // create output mix
        const SLInterfaceID outputMixIIDs[] = {};
        const SLboolean outputMixReqs[] = {};
        result = (*_engineEngine)->CreateOutputMix(_engineEngine, &_outputMixObject, 0, outputMixIIDs, outputMixReqs);           
        if(SL_RESULT_SUCCESS != result){ ERRORLOG("create output mix fail"); break; }

        // realize the output mix
        result = (*_outputMixObject)->Realize(_outputMixObject, SL_BOOLEAN_FALSE);
        if(SL_RESULT_SUCCESS != result){ ERRORLOG("realize the output mix fail"); break; }

        ret = true;
    }while (false);

    return ret;
}

int AudioEngineImpl::play2d(const std::string &fileFullPath ,bool loop ,float volume)
{
    auto audioId = AudioEngine::INVAILD_AUDIO_ID;

    do 
    {
        if (_engineEngine == nullptr)
            break;

        auto& player = _audioPlayers[currentAudioID];
        auto initPlayer = player.init( _engineEngine, _outputMixObject, fileFullPath, volume, loop);
        if (!initPlayer){
            _audioPlayers.erase(currentAudioID);
            log("%s,%d message:create player for %s fail", __func__, __LINE__, fileFullPath.c_str());
            break;
        }

        audioId = currentAudioID++;
        player._audioID = audioId;

        (*(player._fdPlayerPlay))->RegisterCallback(player._fdPlayerPlay, PlayOverEvent, (void*)this);
        (*(player._fdPlayerPlay))->SetCallbackEventsMask(player._fdPlayerPlay, SL_PLAYEVENT_HEADATEND);

        AudioEngine::_audioIDInfoMap[audioId].state = AudioEngine::AudioState::PLAYING;
    } while (0);

    return audioId;
}

void AudioEngineImpl::playerFinishCallback(SLPlayItf caller, SLuint32 playEvent)
{
    auto itend = _audioPlayers.end();
    for (auto iter = _audioPlayers.begin(); iter != itend; ++iter)
    {
        if (iter->second._fdPlayerPlay == caller)
        {
            if (iter->second._finishCallback)
            {
                iter->second._finishCallback(iter->second._audioID, *AudioEngine::_audioIDInfoMap[iter->second._audioID].filePath); 
            }
            AudioEngine::remove(iter->second._audioID);
            _audioPlayers.erase(iter);
            break;
        }
    }
}

void AudioEngineImpl::setVolume(int audioID,float volume)
{
    auto& player = _audioPlayers[audioID];
    int dbVolume = 2000 * log10(volume);
    if(dbVolume < SL_MILLIBEL_MIN){
        dbVolume = SL_MILLIBEL_MIN;
    }
    auto result = (*player._fdPlayerVolume)->SetVolumeLevel(player._fdPlayerVolume, dbVolume);
    if(SL_RESULT_SUCCESS != result){
        log("%s error:%lu",__func__, result);
    }
}

void AudioEngineImpl::setLoop(int audioID, bool loop)
{
    auto& player = _audioPlayers[audioID];
    SLboolean loopEnabled = SL_BOOLEAN_TRUE;
    if (!loop){
        loopEnabled = SL_BOOLEAN_FALSE;
    }
    (*player._fdPlayerSeek)->SetLoop(player._fdPlayerSeek, loopEnabled, 0, SL_TIME_UNKNOWN);
}

void AudioEngineImpl::pause(int audioID)
{
    auto& player = _audioPlayers[audioID];
    auto result = (*player._fdPlayerPlay)->SetPlayState(player._fdPlayerPlay, SL_PLAYSTATE_PAUSED);
    if(SL_RESULT_SUCCESS != result){
        log("%s error:%lu",__func__, result);
    }
}

void AudioEngineImpl::resume(int audioID)
{
    auto& player = _audioPlayers[audioID];
    auto result = (*player._fdPlayerPlay)->SetPlayState(player._fdPlayerPlay, SL_PLAYSTATE_PLAYING);
    if(SL_RESULT_SUCCESS != result){
        log("%s error:%lu",__func__, result);
    }
}

void AudioEngineImpl::stop(int audioID)
{
    auto& player = _audioPlayers[audioID];
    auto result = (*player._fdPlayerPlay)->SetPlayState(player._fdPlayerPlay, SL_PLAYSTATE_STOPPED);
    if(SL_RESULT_SUCCESS != result){
        log("%s error:%lu",__func__, result);
    }

    _audioPlayers.erase(audioID);
}

void AudioEngineImpl::stopAll()
{
    auto itEnd = _audioPlayers.end();
    for (auto it = _audioPlayers.begin(); it != itEnd; ++it)
    {
        auto result = (*it->second._fdPlayerPlay)->SetPlayState(it->second._fdPlayerPlay, SL_PLAYSTATE_STOPPED);
    }
    _audioPlayers.clear();
}

float AudioEngineImpl::getDuration(int audioID)
{
    SLmillisecond duration;
    auto& player = _audioPlayers[audioID];
    auto result = (*player._fdPlayerPlay)->GetDuration(player._fdPlayerPlay, &duration);
    if (duration == SL_TIME_UNKNOWN){
        return AudioEngine::TIME_UNKNOWN;
    }
    else{
        player._duration = duration / 1000.0;

        if (player._duration <= 0)
        {
            return AudioEngine::TIME_UNKNOWN;
        }

        return player._duration;
    }
}

float AudioEngineImpl::getCurrentTime(int audioID)
{
    SLmillisecond currPos;
    auto& player = _audioPlayers[audioID];
    (*player._fdPlayerPlay)->GetPosition(player._fdPlayerPlay, &currPos);
    return currPos / 1000.0f;
}

bool AudioEngineImpl::setCurrentTime(int audioID, float time)
{
    auto& player = _audioPlayers[audioID];
    SLmillisecond pos = 1000 * time;
    auto result = (*player._fdPlayerSeek)->SetPosition(player._fdPlayerSeek, pos, SL_SEEKMODE_ACCURATE);
    if(SL_RESULT_SUCCESS != result){ 
        return false;
    }
    return true;
}

void AudioEngineImpl::setFinishCallback(int audioID, const std::function<void (int, const std::string &)> &callback)
{
    _audioPlayers[audioID]._finishCallback = callback;
}

#endif