### 2026-08-20
this is the first devlog entry. i figured having this log file would help with
documenting the development process of this passion project :)

refactored parts of the code base:
- removed the custom `my_` prefix from commands since it was annoying to type out
- split frame store and variable store source code and make them sub-components of an 
  overarching memory manager component
- add better memory management with cleanup `deinit` functions
- other small cleanup tasks

current fully-fledged functionalities:
- shell commands
- scheduling with different policies via the `exec` command
- demand paging is fully implemented with LRU frame eviction
