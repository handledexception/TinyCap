# TinyCap
Simple, work in progress, D3D11 screen recording app. Currently a project for learning Windows graphics programming and video encoding.

done:
- basic d3d11 orthographic (2D) renderer
- capture primary display via DXGI Desktop Duplication API
- record entire canvas to disk in H.264 and WebM (work in progress with Media Foundation)

todo:
- add performance counter timing system to limit FPS
- put each new "scene" on a new thread
- selection, positioning, resizing of Scene objects with the mouse
- add ability to capture from multiple displays at once
- continuous buffering to memory of last N seconds of video

