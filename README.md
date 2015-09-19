# Simulated Shell

Basic simulation of mainly **job control**.  

Usage  
1. Type ```jobs ``` to view all the background jobs *(running as well as stopped)*.  
2. Type ```start x ``` where x is the pid of the stopped background job which you want to run in background.  
3. Type ```stop x ``` where x is the pid of the running background job which you want to stop in background.  
4. Append ``` &``` in the end of command to make the process as background job.  
5. Press ```Ctrl + \ ``` to kill all the jobs *(running as well as stopped)*.  
6. Type ```quit``` to quit the main program.  

Note  
* Main program is itself immune to ```Ctrl + C``` ,  ```Ctrl + Z ``` , ```Ctrl + \```.
* To stop a foreground job running press ```Ctrl + C```.

BUG  
* While a foreground job is running pressing ```Ctrl + Z ``` sends *SIGTSTP* to the process but doesn't returns the control back to the main program which makes the main program hang. In this case start another terminal and send *SIGKILL* to the foreground job from there.
