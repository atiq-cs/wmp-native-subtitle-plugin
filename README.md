wmp-native-subtitle-plugin
==========================
Recent editions of Windows Media Player uses Microsoft Media Foundation. Media Foundation has some advantages and performance improvements. Using a directshow filter to render captions with Media Foundation hurts that advancement. Performance issue is noticeable sometimes. It is why, our target is to enable captions in WMP for some unsupported subtitle formats without altering media foundation pipeline. 

*Our target is to build a plugin for Windows Media Player that will:*
  * Create supported SAMI caption from unsupported format
  * Enable displaying this caption while playing the video
  * Provide settings to configure this plugin 

Because of the plugin we'll be able to load captions with Video when opened with Media Player.
