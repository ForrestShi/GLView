Purpose
--------------

GLView is a collection of classes designed to make it as easy as possible to get up and running with OpenGL functionality within an iOS app.

Specifically, the GLImage and GLImageView classes make it possible to load and display PVR formatted images and video clips in your app without needing to know any OpenGL whatsoever. See more about PVR images and video below.


Installation
---------------

To use GLView, just drag the class files into your project and add the QuartzCore framework.


Classes
---------------

The GLView library currently includes the following classes:

- GLView - this is a general-purpose UIView subclass for displaying OpenGL graphics on screen. It doesn't display anything by default, but if you are familiar with OpenGL you can use it for raw OpenGL drawing.

- GLImage - this is a class for loading image files as OpenGL textures. It supports all the same image formats as UIImage, as well as a number of PVR image formats.

- GLImageView - this is a subclass of GLView, specifically designed for displaying GLImages. It behaves in a similar way to UIImageView and mostly mirrors its interface.


GLView methods
----------------

GLView has the following methods:

	- (void)bindFramebuffer;

Call this method to bind the GLView before attempting any drawing using OpenGL commands. This would typically be called at the beginning of a drawing loop that executes every frame, or every time a view needs to be redrawn.

	- (BOOL)presentFramebuffer;

Call this method to update the view and display the result of any previous drawing commands. This would typically be called at the end of a drawing loop.


GLImage properties
-------------------

	@property (nonatomic, readonly) CGSize size;
	
The size of the image, in points. As with UIImage, on a retina display device the actual pixel dimensions may be twice the size, depending on the scale property.
	
	@property (nonatomic, readonly) CGFloat scale;

The image scale. For @2x images on retina display devices this will have a value of 2.0. On iOS3.x this will always return 1.0;


GLImage methods
------------------

	+ (GLImage *)imageNamed:(NSString *)name;
	
This method works more-or-less like the equivalent UIImage method. The image with the matching name in the application resources bundle will be loaded and returned. The image is also cached so that any subsequent `imageNamed` calls for the same file will return the exact same copy of the image. IN a low memory situation, this cache will be cleared. Retina display images using the @2x naming scheme also behave the same way as for UIImage.

The name can include a file extension. If it doesn't, .png is assumed. The name may also include a full or partial file reference, and, unlike UIImage's version, GLIMage's imageNamed function can load images outside of the application bundle, such as from the application documents folder. Note however that because these images are cached, it is unwise to load images in this way if they are likely to change or be deleted, as this may result in unpredictable behaviour.

**NOTE:** Currently, GLImage can only load images whose dimensions are powers of two, e.g. 8x128, 32x64, 128x128, 512x256, etc. This is due to technical limitations of OpenGL textures. If you attempt to load a non-power-of-two image, GLImage will fail. If you are using PVR images, the width and height must also be equal (see notes under PVR section below). 
	
	+ (GLImage *)imageWithContentsOfFile:(NSString *)path;
	- (GLImage *)initWithContentsOfFile:(NSString *)path;

These methods load a GLImage from a file. The path parameter can be a full or partial path. For partial paths it is assumed that the path is relative to the application resource folder. If the file extension is omitted it is assumed to be .png. Retina display images using the @2x naming scheme behave the same way as for UIImage. Images loaded in this way are not cached or de-duplicated in any way.
	
	+ (GLImage *)imageWithUIImage:(UIImage *)image;
	- (GLImage *)initWithUIImage:(UIImage *)image;
	
These methods create a GLImage from an existing UIImage. The original scale and orientation of the UIImage are preserved. The result GLIImage format will be a 32 bit uncompressed RGBA texture.
	
	- (void)bindTexture;
	
This method is used to bind the underlying OpenGL texture of the GLImage prior to using it for OpenGL drawing.
	
	- (void)drawAtPoint:(CGPoint)point;
	
This method will draw the image into the currently bound GLView or OpenGL context at the specified point.
	
	- (void)drawInRect:(CGRect)rect;

This method will draw the image into the currently bound GLView or OpenGL context, stretching it to fit the specified CGRect.


GLImageView properties
-----------------------

The GLImage view has the following properties:

	@property (nonatomic, retain) GLImage *image;

This is used to set and display a GLImage within the view. The standard UIView `contentMode` property is respected when drawing the image, in terms of how it is cropped, stretched and positioned within the view.

	@property (nonatomic, copy) NSArray *animationImages;
	
This is used to specify a sequence of images to be played as an animation. The array can contain either GLImages, or filenames. If filenames are supplied then the images will be streamed from disk rather than being loaded in advance. See the example app for a demonstration of how this can be used to play a large PVR video sequence with smooth 30fps performance and minimal memory consumption. 
	
	@property (nonatomic, assign) NSTimeInterval animationDuration;

The duration over which the animationImages sequence will play. This is measured in seconds and defaults to the number of animation frames divided by 30 (i.e. 30 frames-per-second). This value is automatically set whenever the animationImages array is updated, so remember to set this *after* you set the animationImages property.

	@property (nonatomic, assign) NSInteger animationRepeatCount;

The number of times the animation will repeat. Defaults to zero, which causes the animation to repeat indefinitely until the `stopAnimating` method is called.


GLImageView methods
-----------------------

	- (GLImageView *)initWithImage:(GLImage *)image;
	
This method creates a GLImageView containing the specified image and sets the view frame to match the image size.

	- (void)startAnimating;
	
This method starts the `animationImages` sequence. Playback always starts from the first frame. Calling play when the animation is already playing will start it again from the beginning.
	
- (void)stopAnimating;

This method stops the animation sequence. Once stopped, the sequence can not be resumed, only started again from the beginning.

- (BOOL)isAnimating;

This method returns YES if the `animationImages` sequence is currently playing.


PVR images
--------------

The PVR image format is a special proprietary image format used by the PowerVR graphics chip in iOS devices. PVR images load quicker and can take up less space in memory than PNGs, so they can be very useful for apps that need to load and display a lot of images.

Due to the rapid loading, they can also be streamed off disk fast enough to create movie clips, which can be a handy way to display video if the standard movie player APIs don't meet your requirements (e.g. if you need video with transparent portions).

You will probably notice when creating PVRs that the file size is actually larger than the equivalent PNG file. This is misleading however - PNG files use internal zip compression which makes them small on disk, but when they are loaded into memory they are expanded to consume an amount of space equivalent to their width * height * 4 bytes.

The same is true of JPEG or any other image format. But with PVR, the size on disk matches the size in memory, so a 16-bit PVR only consumes half as much memory as a PNG of the same dimensions, and a compressed PVR takes up even less space. Since the structure on disk is exactly the same as in memory, PVRs also load quicker because there is no need to decompress or transcode them.

PVRs are restricted to square images with power-of-two sizes however. See the note below about conversion.
	

Generating PVR image files
----------------------------

GLImage can load PVR images with any of the following pixel formats:

- RGBA8888 - 32 bit, high quality 24-bit colour with 8-bit alpha
- RGBA4444 - 16 bit, 12-bit colour and 4-bit alpha
- RGBA5551 - 16 bit, higher precision 5-bit color but only 1-bit alpha
- RGB565 - 16 bit, higher precision color, but no alpha transparency
- PVRTC4 - 4 bits-per-pixel lossy compression, with or without alpha
- PVRTC2 - 2 bits-per-pixel lossy compression, with or without alpha

Apple includes a command-line PVT texture generation application called texturetool with the Xcode developer tools. This can usually be found at:

/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/texturetool

The texturetool application is fairly limited, and can only be used to create 4bpp (bits-per-pixel) and 2bpp compressed images, which are extremely low quality and look a bit like highly compressed jpegs. These are probably not good enough for still images - especially images containing transparency (the PVR OpenGL logo in the example demonstrates this) - but may be appropriate for video frames or textures for 3D models.

**NOTE:** In addition to needing power-of-two dimensions, PVR images must also be perfectly square, i.e. the width and height must be equal. Valid sizes are 2x2, 4x4, 8x8, 16x16, 32x32, 64x64, 128x128, 256x256, etc. Remember to crop or scale your images to a valid size before converting them.

The typical texturetool settings you will want to use are one of the following:

	/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/texturetool -e PVRTC --channel-weighting-perceptual --bits-per-pixel-4 -f PVR -o {output_file_name}.pvr {input_file_name}.png

This generates a 4 bpp compressed PVR image.

	/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/texturetool -e PVRTC --channel-weighting-perceptual --bits-per-pixel-2 -f PVR -o {output_file_name}.pvr {input_file_name}.png
	
This generates a 2 bpp compressed PVR image.

As stated previously, these files will appear like heavily compressed JPEG images, and will not be appropriate for user interface components or images with fine detail. It is also possible to create PVR images in a variety of higher qualities, but for this you will need to use a different tool. One such app is TexturePacker (http://www.texturepacker.com/), which is not free, but provides a handy command line tool for generating additional PVR formats.

The typical TexturePacker settings you will want to use are one of the following:

	TexturePacker --disable-rotation --no-trim --opt RGBA8888 {input_file_name}.png --sheet {output_file_name}.pvr
	
This creates a 32-bit maximum-quality PVR image with alpha transparency.

	TexturePacker --disable-rotation --no-trim --dither-fs-alpha --opt RGBA4444 {input_file_name}.png --sheet {output_file_name}.pvr
	
This creates a 16-bit dithered PVR image with alpha transparency.

	TexturePacker --disable-rotation --no-trim --dither-fs --opt RGB565 {input_file_name}.png --sheet {output_file_name}.pvr
	
This creates an opaque 16-bit dithered PVR image without alpha transparency.


Generating PVR video clips
----------------------------

To use the GLImageView as a PVR video player, you'll need to convert your video into a sequence of PVR images. Assuming you are starting with a Quicktime-compatible video file, follow these steps. If you've already got your video as a sequence of numbered PNG or JPEG images then you can skip to step 4.

1) Open your video in QuickTime 7 (not Quicktime X). You'll need a QuickTime Pro license to do anything useful.

2) Assuming the video isn't already a square with power-of-two sides, you first need to convert it to the correct aspect ratio. Select Export... and choose 'Movie to MPEG-4'. Within this interface you can select a custom width and height. Go for the nearest square power-of-two dimensions to the actual video size, and you're probably better off choosing to crop or scale the image than letterbox it. Go for the highest quality you can to avoid compression artifacts.

3) After exporting your video as an MP4, open the new video in QuickTime 7 and go to the export option again. This time select 'Movie to Image Sequence' and export the video as a sequence of PNG files.

4) Now that you have the individual frames, you'll need to convert them to PVRs. Using texturetool or TexturePacker you can batch convert the images. Here are some example command line options:

	find {image_directory} -name \*.png | sed 's/\.png//g' | xargs -I % -n 1 /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/texturetool -e PVRTC --channel-weighting-perceptual --bits-per-pixel-4 -f PVR -o %.pvr %.png
	
This generates the frames as 4 bpp compressed PVR images (This will take a while). You can do the same with TexturePacker as follows:

	find {image_directory} -name \*.png | sed 's/\.png//g' | \
    xargs -I % -n 1 TexturePacker %.png --disable-rotation --no-trim --dither-fs-alpha --opt RGBA4444 %.png --sheet %.pvr

This would create the frames as maximum-quality 32-bit PVR images with alpha transparency.

**NOTE:** PVR image sequences are very large compared with the original MP4 movie, or even the equivalent PNG image sequence. PVR is optimised for memory usage and loading speed, not disk space, so be prepared for your app to grow dramatically if you include a lot of PVR video frames.