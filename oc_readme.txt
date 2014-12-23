///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Outcast Lives Forever project by AND ( aka Dmitry Andreev ) 2002-2005
//   wab   : http://www.and.intercon.ru
//   email : andcoder@gmail.com
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Original OUTCAST game by Appeal
//   web   : http://www.appeal.be
//   web   : http://www.fresh3d.com
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Description :
//
//      OC_viewer.exe is experemental engine/viewer
//      of original OUTCAST's resources like landscapes, bitmaps,
//      sounds, fonts, meshes, characters etc...
//      NOTE : control keys are
//       W - forward, S - backward, A - left step, D - right step, Escape - main menu
//       and mouse for look around.
//      
//      OC_data.exe is a console version of resource convertor.
//      It has the same shared code as OC_viewer.exe but
//      provides more flexible tools for ripping resources from OC.
//
//      OC_unpack.exe is extractor of original .pak files.
//
//      OC_pack.exe is OC PAK Builder utility.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

dd.mm.yyyy

--- v1.02 ---

25.04.2005 - Fixed : Crash on player selection. CBtmSurface out of array fix.
             Fixed : Per-pixel fog fixed. Common projection matrix turned to W-Friendly matrix.
             New   : Two animations cross blending. Ugly looking code has to be rewrote.
             New   : Improoved preloader with more smart resolution option and colorfull icons.
             New   : OC PAK builder without compression.
             New   : Hardware cursor with animation effects.

--- v1.01 ---

10.12.2003 - Fixed : A lot of memory leaks have been killed
             Fixed : Memory optimized

05.12.2003 - New   : Soft shadows from characters like in original game (projected + blured)
             Fixed : Ugly bug during characters rendering
             ToDo  : Preloader with resolution and other graphical options on start
             ToDo  : Animation cross blending

02.12.2003 - New   : Outcast like bump mapping on characters works greate (on PS1.1 thru EMBM)

30.11.2003 - New   : Collision detection with landscape
             New   : 3rd person view
             New   : New fixed timer system to avoid scene quaking on little time delta
             New   : Player selection stuff thru player entities
             New   : New menu option for player selection

23.11.2003 - Hacked: .anm file has been hacked
             New   : Animation pipeline works just fine, except that Appeal's skeletal system
                   : was uncommon today, so, and I don't sure about all coefficients, but
                   : in general all animation work the same
             ToDo  : OC like bump mapping on characters
             ToDo  : Entity system for characters
             ToDo  : UI for several kind of game (free camera, 3rd camera, player selection...)

21.11.2003 - Hacked: .amh file has been hacked
             New   : Now, OC works on DirectX 9 with advanced pixel shaders
             New   : Post processing stuff works just fine, on pixel shaders of course ;)
             New   : Decay texture has been added on water surface, now it looks much better

16.11.2003 - ToDo  : Hack .amh and .anm files to get up all characters
             ToDo  : Port it under DirectX 9 to get more features
             ToDo  : Some post-processing effects
             ToDo  : Collision detection of AABB with landscape
             ToDo  : Third person camera and controlling
             ToDo  : Fixup water waves by wave decal map
             ToDo  : Map reload
             ToDo  : Fixup tile edges
             ToDo  : To think about vertical mapping on tiles

---------------------------------------------------------------------------------------------------
Yaaaaahhhhh!!!!!! I am back, and I am gonna make new features for the project.
---------------------------------------------------------------------------------------------------

01.12.2002 - New   : It is a first public version
             New   : More interactive menu
             New   : A lot of debug information in LOG file
             Fixed : Some little bugs in file system

25.11.2002 - New   : Now OC_viewer has OC like menu interface
             New   : OC like water surface ( without Pixel/Vertex Shaders )
             New   : OC like perpixel layered fog
             New   : Working with OC's PAK files directly
             Hacked: PKWARE's DCL exploding code was reverse engineered from PKZIP25
             Hacked: .fnt file was completly hacked
             New   : project has new tool for converting OC's fonts into internal format
             New!!!: Rendering of dynamic decorations (entities) was released, BUT Appeal has
                   : a lot of hacks for it in a code, for example a lot of meshes for entities
                   : loads directly from a code, etc... It looks like sux, and it was disabled

16.11.2002 - New   : Outcast like sky mapping released with correct fog color
             New   : correct rendering (with textures, etc ) of static decorations (not entities)
             Hacked: .msh format was completly hacked
             ToDo  : well, I want to see Outcast like water surfaces in high quality
             ToDo  : I need hack .fnt files and make Outcast like UI
             ToDo  : correct mapping and tesselation of vertical mapped emulated voxels

12.11.2002 - Hacked: .wrd file was completly hacked
             Hacked: origin of world was found and decoded ( in .lnd file, not in .wrd )
             ToDo  : do colorfull rendering of decorations with textures etc

11.11.2002 - Hacked: .wrd file was hacked ( only decorations part )
             New   : now it can renders on level polygonal decorations ( without textures )
             ToDo  : wtf...? Where is origin of the world, decorations must be on their places

08.11.2002 - Fixed : particulary removed spaces between tiles
             Fixed : mipmap blurity on edges of a tiles
             Fixed : correct frustum culling and front to back sorting
             Hacked: part of .lnd file wich contain palette of height of water surfaces
             Hacked: part of .msh (static mesh) wich contain set of vertexes and faces
             New   : correct working of wake water and range base fog
             New   : optimized for higher FPS and polygon count ( 50000-10000 tris on 250-320 fps )
             Hacked: completly hacked .rbt file format ( packed grayscaled 8bit bitmaps )
             ToDo  : space remover algorythm must be more complexety

03.11.2002 - New   : It is working at the moment ;)
             New   : input system present and it can control player
             New   : base player was added to OCWorld
             ToDo  : remove spaces between tiles, do frustum optimization,
             do lod optimization, working with original PAKs,
             do correct PTexturedVoxelTile tesselation with first 16 colors
             and vertical voxel mapping, draw fake water, draw skies,
             something else ?...

01.11.2002 - New   : OC3D engine has base classes
             New   : XMath (my math library) was included into project
             ToDo  : let's make DX base input system

28.10.2002 - New   : new adaptive tesselator works greate :)
             New   : OC_viewer was started ! ( it has a lot of Squish 64k code ;)

19.10.2002 - Fixed : height map of each tile extraction
             Fixed : 11.pak has .lnd wich contain new PFlatVoxelTile record
             Hacked: .ftl ( flat voxel tile ) was hacked
             New   : new approach for adaptive tesselation was found :)
             ToDo  : let's research it
             ToDo  : I need some interactive 3D viewer, let's do it

08.09.2002 - New   : well, tesselator works but it has some stupid bugs, I don't like it
             ToDo  : let's continue working for araptive tesselator

16.08.2002 - Hacked: .lnd file was hacked for simple dumping into one large tga
             Hacked: .tvt ( texture voxel tile ) file was hacked
             Hacked: .vtl ( voxel tile ) file was hacked
             ToDo  : looking for solution for height maps and their precision
             ToDo  : start working for heightmap to tris tesselator

08.08.2002 - Hacked: .btm (planar bitmap with layers) file was completly hacked
             Hacked: .plt (index remapping palette) file was completly hacked too
             ToDo  : hmmm, let's hack .lnd (landscape) file

28.07.2002 - New   : OC_unpack.exe works well
             ToDo  : Now I can start researches for OUTCAST's resources
             ToDo  : Let's start with simple 2D graphic formats

27.07.2002 - Hacked: .pak (packed OC file) was cracked

XX.07.2002 - Project was started, dreams of idiot comes true ;)
