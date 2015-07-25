# Filesystem Exporter Plugin
The Filesystem Exporter Plugin for dedicated servers creates an interface to the filesystem.
This is needed for binary modules that needs to access mountet content or addons on these servers.

## Installation
You put the binary file for your OS (\*.dll or \*.so) and the \*.vdt file in to the garrysmod/addons/ folder.
[You find them in filesys_export/install/](https://github.com/Grocel/GMod_Modules/tree/master/filesys_export/install)

## Note
This is only needed for dedicated servers. Do not install this on your listen server or single player, because it is not needed nor ran here.

## For Developers
Feel free to use it for your projects. To use it in a project you need to load it dynamically and import the GetFilesystem() function.
You should get a function pointer with this structure: `typedef IFileSystem* (*GetFilesystem_t)();`

After importing you can call the function to get the running IFileSystem interface.
Make sure you only try to load the plugin module on a dedicated server as it would not return a valid interface otherwise.