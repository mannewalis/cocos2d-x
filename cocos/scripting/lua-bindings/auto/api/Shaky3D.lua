
--------------------------------
-- @module Shaky3D
-- @extend Grid3DAction
-- @parent_module cc

--------------------------------
-- initializes the action with a range, shake Z vertices, a grid and duration<br>
-- param duration in seconds
-- @function [parent=#Shaky3D] initWithDuration 
-- @param self
-- @param #float duration
-- @param #size_table gridSize
-- @param #int range
-- @param #bool shakeZ
-- @return bool#bool ret (return value: bool)
        
--------------------------------
--  creates the action with a range, shake Z vertices, a grid and duration 
-- @function [parent=#Shaky3D] create 
-- @param self
-- @param #float duration
-- @param #size_table gridSize
-- @param #int range
-- @param #bool shakeZ
-- @return Shaky3D#Shaky3D ret (return value: cc.Shaky3D)
        
--------------------------------
-- 
-- @function [parent=#Shaky3D] clone 
-- @param self
-- @return Shaky3D#Shaky3D ret (return value: cc.Shaky3D)
        
--------------------------------
-- 
-- @function [parent=#Shaky3D] update 
-- @param self
-- @param #float time
-- @return Shaky3D#Shaky3D self (return value: cc.Shaky3D)
        
--------------------------------
-- 
-- @function [parent=#Shaky3D] Shaky3D 
-- @param self
-- @return Shaky3D#Shaky3D self (return value: cc.Shaky3D)
        
return nil
