#include <irrlicht.h>
#include <iostream>

using namespace std;
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;

class example1
{
    int PICSCOUNT = 800;
    void processFolder(IrrlichtDevice * device, const path &newDirectory)
    {
        //Get the File System from the device.
        IFileSystem * fs = device->getFileSystem();

        //Get the Scene Manager from the device.
        ISceneManager * smgr = device->getSceneManager();

        //Get the Video Driver from the device.
        IVideoDriver * driver = device->getVideoDriver();

        //If maximum number of pictures already loaded, then don't load anymore.
        if(driver->getTextureCount() >= PICSCOUNT)
        {
            return;
        }

        //Change working directory.
        fs->changeWorkingDirectoryTo(newDirectory);

        //Get List of files and sub folders.
        IFileList * flist = fs->createFileList();

        //Sort by file names and folder names.
        flist->sort();

        //Loop through the contents of the working directory.
        for(u32 i = 0; i < flist->getFileCount(); i++)
        {
            //If current item is a directory
            if(flist->isDirectory(i))
            {
                //and it is not "Parent Directory .."
                if(strcmp(flist->getFileName(i).c_str(),"..") != 0)
                {
                    //process contents of the current sub directory
                    processFolder(device,flist->getFullFileName(i));
                }
            }
            else //If current item is a file
            {
                //Get file name
                path filename = flist->getFileName(i);

                //Get extension from file name
                std::string extension = filename.subString(filename.size() - 4,4).c_str();

                //If file extension is .png
                if(strcasecmp(extension.data(),".png") == 0)
                {
                    //Create a new cube node with unit dimention
                    ISceneNode * node = smgr->addCubeSceneNode(1.0f);

                    //Scale the cube to the dimentions of our liking - 75x107x0.1
                    node->setScale(vector3df(75,107,0.1f));

                    //Set random X cordinate between -500 and 500
                    long x = random()% 1000 - 500;

                    //Set random Y cordinate between -500 and 500
                    long y = random()% 1000 - 500;

                    //Set random Z cordinate between -500 and 500
                    long z = random()% 1000 - 500;

                    //Create a position vector
                    vector3df pos(x,y,z);

                    //Create a position vector for 500,500,500
                    //vector3df pos(500,500,500);

                    //Change cordinates such that direction is preserved and length is 800 units
                    //pos = pos.normalize() * 800.0f;

                    //Set Y cordinate to 0
                    //pos.Y = 0;

                    //Apply new position to cube
                    node->setPosition(pos);

                    //Get active camera
                    ICameraSceneNode * cam = smgr->getActiveCamera();

                    //Set camera to "look" at cube
                    cam->setTarget(node->getPosition());

                    //Apply camera's new rotation (as a result of above) to the node
                    node->setRotation(cam->getRotation());

                    //Make cube's texture visible without light
                    node->setMaterialFlag(EMF_LIGHTING, false);

                    //Set the file's graphics as texture to the cube
                    node->setMaterialTexture(0,driver->getTexture(flist->getFullFileName(i).c_str()));

                    //If maximum number of pictures already loaded, then don't load anymore.
                    if(driver->getTextureCount() >= PICSCOUNT)
                    {
                        return;
                    }
                }
            }
        }
    }

public:
    int runSimulation()
    {
        //Create an Irrlicht Device.
        IrrlichtDevice * device = createDevice(EDT_OPENGL,dimension2d<u32>(800,600));
        //Get the Scene Manager from the device.
        ISceneManager * smgr = device->getSceneManager();
        //Get the Video Driver from the device.
        IVideoDriver * driver = device->getVideoDriver();
        //Add a Cube to the Scene.
        //ISceneNode * node = smgr->addCubeSceneNode();
        //Needed to make the object's texture visible without a light source.
        //node->setMaterialFlag(EMF_LIGHTING, false);
        //Add texture to the cube.
        //node->setMaterialTexture(0,driver->getTexture("/usr/share/irrlicht/media/wall.jpg"));
        //Set cube 100 units further in forward direction (Z axis).
        //node->setPosition(vector3df(0,0,100));

        //Add FPS Camera to allow movement using Keyboard and Mouse.
        smgr->addCameraSceneNodeFPS();

        //Process contents of this folder.
        processFolder(device, "/home/karim/Images/Cards-Large/150/");

        //Run simulation
        while(device->run())
        {
            //Begin Scene with a gray backdrop #rgb(125,125,125)
            driver->beginScene(true,true,SColor(0,125,125,125));
            //Render the scene at this instant.
            smgr->drawAll();
            //End the scene
            driver->endScene();
            //Logic to update the scene will go here.
        }

        smgr->drop();
        driver->drop();
        device->drop();

        return 0;
    }
};
