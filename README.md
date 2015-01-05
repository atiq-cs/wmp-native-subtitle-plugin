Windows Media Player Native Subtitle Plugin
=============================================
Recent editions of Windows Media Player uses Microsoft Media Foundation. Media Foundation has some advantages and performance improvements. Using a directshow filter to render captions with Media Foundation hurts that advancement. Performance issue is noticeable sometimes. It is why, our target is to enable captions in WMP for some unsupported subtitle formats without altering media foundation pipeline. 

*Our target is to build a plugin for Windows Media Player that will:*
  * Create supported SAMI caption from unsupported format
  * Enable displaying this caption while playing the video
  * Provide settings to configure this plugin 

Enabling this plugin we are able to enable captions with Video when opened with Media Player. For detailed overview on this project please read the [article on codeproject](http://www.codeproject.com/Articles/766246/Windows-Media-Player-Native-Subtitle-Plugin-Cplusp)
