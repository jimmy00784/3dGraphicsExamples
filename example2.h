#include <irrlicht.h>
#include <tokamak.h>
#include <iostream>

using namespace std;
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;

class example2
{
    int PICSCOUNT = 800;
    ISceneNode** sceneNodes;
    neRigidBody** rigidBodies;
    int sceneCount = 0;
    int bodyCount = 0;
    f32 cardmass = 1;
    void processFolder(IrrlichtDevice * device,neSimulator * world, const path &newDirectory)
    {
        IFileSystem * fs = device->getFileSystem();
        ISceneManager * smgr = device->getSceneManager();
        IVideoDriver * driver = device->getVideoDriver();
        if(driver->getTextureCount() >= PICSCOUNT)
        {
            return;
        }
        fs->changeWorkingDirectoryTo(newDirectory);
        IFileList * flist = fs->createFileList();
        flist->sort();
        vector3df cardsize(75,107,1);
        vector3df halfcardsize = cardsize;
        halfcardsize.Z = 0.5;

        for(u32 i = 0; i < flist->getFileCount(); i++)
        {
            if(flist->isDirectory(i))
            {
                if(strcmp(flist->getFileName(i).c_str(),"..") != 0)
                {
                    processFolder(device,world,flist->getFullFileName(i));
                }
            }
            else
            {
                path filename = flist->getFileName(i);
                std::string extension = filename.subString(filename.size() - 4,4).c_str();
                if(strcasecmp(extension.data(),".png") == 0 &&
                        (filename.find("clubs") == 0 ||
                         filename.find("diamonds") == 0 ||
                         filename.find("spades") == 0 ||
                         filename.find("hearts") ==0))
                {
                    ISceneNode * node = smgr->addCubeSceneNode(1.0f);
                    sceneNodes[sceneCount] = node;
                    node->setScale(halfcardsize);
                    long x = random()% 1000 - 500;
                    long y = random()% 1000 - 500;
                    long z = random()% 1000 - 500;
                    vector3df pos(x,y,z);
                    node->setPosition(pos);
                    ICameraSceneNode * cam = smgr->getActiveCamera();
                    cam->setTarget(node->getPosition());
                    //node->setRotation(cam->getRotation());
                    node->setMaterialFlag(EMF_LIGHTING, false);
                    node->setMaterialTexture(0,driver->getTexture(flist->getFullFileName(i).c_str()));

                    neRigidBody * rigid = world->CreateRigidBody();
                    neGeometry * geom = rigid->AddGeometry();
                    geom->SetBoxSize(irrVectorToTokamak(cardsize));
                    rigid->UpdateBoundingInfo();

                    rigid->SetMass(cardmass);
                    rigid->SetInertiaTensor(neBoxInertiaTensor(irrVectorToTokamak(halfcardsize),cardmass));
                    rigid->SetPos(irrVectorToTokamak(pos));
                    vector3df rot = cam->getRotation();
                    quaternion rotQ(rot);
                    rotQ *= PI/180;
                    neQ neRot(rotQ.X,rotQ.Y,rotQ.Z,rotQ.W);
                    rigid->SetRotation(neRot);

                    rigid->SetUserData(sceneCount);
                    rigidBodies[bodyCount] = rigid;
                    bodyCount++;
                    sceneCount++;
                    if(driver->getTextureCount() >= PICSCOUNT)
                    {
                        return;
                    }
                }
            }
        }
    }
    neV3 irrVectorToTokamak(vector3df& vec)
    {
        neV3 result;
        result.Set(vec.X,vec.Y,vec.Z);
        return result;
    }

    vector3df tokVectorToIrrlicht(neV3& vec)
    {
        vector3df result(vec.X(),vec.Y(),vec.Z());
        return result;
    }

public:
    example2()
    {
        sceneNodes = new ISceneNode*[PICSCOUNT];
        rigidBodies = new neRigidBody*[PICSCOUNT];
    }

    int runSimulation()
    {
        IrrlichtDevice * device = createDevice(EDT_OPENGL,dimension2d<u32>(800,600));
        ISceneManager * smgr = device->getSceneManager();
        IVideoDriver * driver = device->getVideoDriver();
        ITimer * irrTimer = device->getTimer();
        smgr->addCameraSceneNodeFPS();
        neSimulatorSizeInfo siminfo;
        neV3 gravity;
        gravity.Set(0,-10,0);
        siminfo.rigidBodiesCount = 100;
        siminfo.animatedBodiesCount = 1;
        int totalCount = siminfo.animatedBodiesCount + siminfo.rigidBodiesCount;
        siminfo.geometriesCount = totalCount;
        siminfo.overlappedPairsCount = totalCount * (totalCount - 1) / 2;
        neSimulator* world = neSimulator::CreateSimulator(siminfo,NULL,&gravity);

        neAnimatedBody * floor = world->CreateAnimatedBody();
        neGeometry* geom = floor->AddGeometry();
        geom->SetBoxSize(1500,50,1500);
        floor->UpdateBoundingInfo();
        neV3 pos;
        pos.Set(0,-600,0);
        floor->SetPos(pos);

        ISceneNode * node = smgr->addCubeSceneNode(1);
        node->setScale(vector3df(1500,50,1500));
        node->setPosition(tokVectorToIrrlicht(pos));
        node->setMaterialFlag(EMF_LIGHTING,false);
        node->setMaterialTexture(0,driver->getTexture("/usr/share/irrlicht/media/wall.jpg"));
        processFolder(device,world, "/home/karim/Images/Cards-Large/150/");

        u32 TimeStamp = irrTimer->getTime();
        f32 DeltaTime = 0, LastDelta = 0;
        u32 CurrentTime = TimeStamp;
        while(device->run())
        {
            LastDelta = DeltaTime;
            CurrentTime = irrTimer->getTime();
            DeltaTime = CurrentTime - TimeStamp;
            TimeStamp = CurrentTime;
            if(LastDelta != 0)
            {
                if(DeltaTime > LastDelta * 1.2f) DeltaTime = LastDelta * 1.2f;
                if(DeltaTime < LastDelta * 0.8f) DeltaTime = LastDelta * 0.8f;
            }

            if(DeltaTime > 1.0f / 45.0f) DeltaTime = 1.0f/45.0f;

            for(int i = 0; i < bodyCount; i++)
            {
                neRigidBody * body = rigidBodies[i];
                ISceneNode * node = sceneNodes[body->GetUserData()];
                neV3 bodyPos = body->GetPos();
                vector3df nodePos = tokVectorToIrrlicht(bodyPos);
                node->setPosition(nodePos);
                neQ bodyRot = body->GetRotationQ();

                quaternion nodeRotQ;
                nodeRotQ.set(bodyRot.X,bodyRot.Y,bodyRot.Z,bodyRot.W);
                vector3df nodeRotE;
                nodeRotQ.toEuler(nodeRotE);
                nodeRotE *= 180/PI;
                node->setRotation(nodeRotE);
            }

            driver->beginScene(true,true,SColor(0,125,125,125));
            smgr->drawAll();
            driver->endScene();
            world->Advance(DeltaTime);

        }

        smgr->drop();
        driver->drop();
        device->drop();

        delete rigidBodies;
        delete sceneNodes;

        if(world)
        {
            neSimulator::DestroySimulator(world);
            world = NULL;
        }

        return 0;
    }
};
