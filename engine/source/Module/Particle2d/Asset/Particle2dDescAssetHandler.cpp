#include "Particle2d/Asset/Particle2dDescAssetHandler.h"

#include "Particle2d/Priv/Particle2dTypes.h"

#include "AzCore/JSON/document.h"
#include "AzCore/JSON/stringbuffer.h"

namespace Module
{
	bool Particle2dDescAssetHandler::OnLoadAssetData(const AZ::Data::Asset<AZ::Data::AssetData>& asset, void* buffer, size_t length)
	{
		Particle2dDescAsset* assetData = asset.GetAs<Particle2dDescAsset>();

		using namespace rapidjson;
		using namespace Particle2d;

		Document document;
		StringStream s((char*)buffer);
		document.ParseStream(s);
		Value& structListPtr = document["Particle2DStructInfoList"];
		if (structListPtr.IsArray())
		{
			for (int i = 0; i < structListPtr.Size(); i++)
			{
				Particle2DStructData* p2dStructDataPtr = new Particle2DStructData;
				p2dStructDataPtr->Index = structListPtr[i]["Index"].GetInt();
				Value& childListPtr = structListPtr[i]["ChildIndexList"];
				if (childListPtr.IsArray())
				{
					for (int j = 0; j < childListPtr.Size(); j++)
					{
						p2dStructDataPtr->ChildIndexArray.push_back(childListPtr[j].GetInt());
					}
				}
				assetData->m_desc.LauncherStruct.push_back(p2dStructDataPtr);
			}
		}
		Value& dataListPtr = document["Particle2DLauncherDataInfoList"];
		if (dataListPtr.IsArray())
		{
			for (int i = 0; i < dataListPtr.Size(); i++)
			{
				Particle2DLauncherData* p2dLauncherDataPtr = new Particle2DLauncherData;

				p2dLauncherDataPtr->Offset.SetX((float)dataListPtr[i]["Offset"][0].GetDouble());
				p2dLauncherDataPtr->Offset.SetY((float)dataListPtr[i]["Offset"][1].GetDouble());

				p2dLauncherDataPtr->DelayTime = (float)dataListPtr[i]["DelayTime"].GetDouble();
				p2dLauncherDataPtr->MaxParticlesNum = dataListPtr[i]["MaxParticlesNum"].GetInt();                        //    cJSON_GetObjectItem(dataPtr, "")->valueint;
				p2dLauncherDataPtr->LayerIndex = dataListPtr[i]["LayerIndex"].GetInt();                                        //    cJSON_GetObjectItem(dataPtr, "")->valueint;
				p2dLauncherDataPtr->RateOverTime = dataListPtr[i]["RateOverTime"].GetInt();                                     //    cJSON_GetObjectItem(dataPtr, "")->valueint;
				p2dLauncherDataPtr->PerUnitTime = (float)(1.0f / (float)p2dLauncherDataPtr->RateOverTime);
				p2dLauncherDataPtr->Duration = (float)dataListPtr[i]["Duration"].GetDouble(); // (float)cJSON_GetObjectItem(dataPtr, "")->valuedouble;
				p2dLauncherDataPtr->Loop = dataListPtr[i]["Loop"].GetBool();           //      cJSON_GetObjectItem(dataPtr, "")->valueint != 0;

				p2dLauncherDataPtr->UnitMinLifetime = (float)dataListPtr[i]["UnitMinLifetime"].GetDouble();      //              (float)cJSON_GetObjectItem(dataPtr, "")->valuedouble;
				p2dLauncherDataPtr->UnitMaxLifeTime = (float)dataListPtr[i]["UnitMaxLifeTime"].GetDouble();      //            (float)cJSON_GetObjectItem(dataPtr, "")->valuedouble;
				p2dLauncherDataPtr->UnitLifeTimeRandom = dataListPtr[i]["UnitLifeTimeRandom"].GetBool();       //         cJSON_GetObjectItem(dataPtr, "")->valueint != 0;
				p2dLauncherDataPtr->UnitLifeTime = (float)dataListPtr[i]["UnitLifeTime"].GetDouble();           //           (float)cJSON_GetObjectItem(dataPtr, "")->valuedouble;

				p2dLauncherDataPtr->ParticleLauncherType = dataListPtr[i]["ParticleLauncherType"].GetInt();                                                   // cJSON_GetObjectItem(dataPtr, "")->valueint;
				p2dLauncherDataPtr->LauncherShootAngle = (float)dataListPtr[i]["LauncherShootAngle"].GetDouble();                                             //     (float)cJSON_GetObjectItem(dataPtr, "")->valuedouble;
				p2dLauncherDataPtr->LauncherShootRandomNum = (float)dataListPtr[i]["LauncherShootRandomNum"].GetDouble();                                  //      (float)cJSON_GetObjectItem(dataPtr, "")->valuedouble;
				p2dLauncherDataPtr->LauncherShootLength = (float)dataListPtr[i]["LauncherShootLength"].GetDouble();                            // (float)cJSON_GetObjectItem(dataPtr, "")->valuedouble;
				p2dLauncherDataPtr->LauncherShootDepth = (float)dataListPtr[i]["LauncherShootDepth"].GetDouble();                           // (float)cJSON_GetObjectItem(dataPtr, "")->valuedouble;
				p2dLauncherDataPtr->CirculeNotRandomDir = dataListPtr[i]["CirculeNotRandomDir"].GetBool();                                // cJSON_GetObjectItem(dataPtr, "")->valueint != 0;

				p2dLauncherDataPtr->UseSpeedCurve = dataListPtr[i]["UseSpeedCurve"].GetBool();         // cJSON_GetObjectItem(dataPtr, "")->valueint != 0;
				if (p2dLauncherDataPtr->UseSpeedCurve)
				{
					Value& SpeedKeyFrameListPtr = dataListPtr[i]["SpeedKeyFrameList"];
					if (SpeedKeyFrameListPtr.IsArray())
					{
						for (int j = 0; j < SpeedKeyFrameListPtr.Size(); j++)
						{
							FrameFloatPair* frameFloatPair = new FrameFloatPair;
							frameFloatPair->Key = (float)SpeedKeyFrameListPtr[j]["Key"].GetDouble();
							frameFloatPair->Value = (float)SpeedKeyFrameListPtr[j]["Value"].GetDouble();
							p2dLauncherDataPtr->SpeedKeyFrameArray.push_back(frameFloatPair);
						}
					}
				}
				p2dLauncherDataPtr->UnitMinSpeed = (float)dataListPtr[i]["UnitMinSpeed"].GetDouble();//                       cJSON_GetObjectItem(dataPtr, "UnitMinSpeed")->valuedouble;
				p2dLauncherDataPtr->UnitMaxSpeed = (float)dataListPtr[i]["UnitMaxSpeed"].GetDouble();        // (float)cJSON_GetObjectItem(dataPtr, "UnitMaxSpeed")->valuedouble;
				p2dLauncherDataPtr->UnitSpeed = (float)dataListPtr[i]["UnitSpeed"].GetDouble();  //(float)cJSON_GetObjectItem(dataPtr, "UnitSpeed")->valuedouble;
				p2dLauncherDataPtr->UnitSpeedRandom = dataListPtr[i]["UnitSpeedRandom"].GetBool();                 // cJSON_GetObjectItem(dataPtr, "UnitSpeedRandom")->valueint != 0;
				p2dLauncherDataPtr->useForce = dataListPtr[i]["useForce"].GetBool();     //             cJSON_GetObjectItem(dataPtr, "useForce")->valueint != 0;
				p2dLauncherDataPtr->ForceDir = (float)dataListPtr[i]["ForceDir"].GetDouble();   //   (float)cJSON_GetObjectItem(dataPtr, "ForceDir")->valuedouble;
				p2dLauncherDataPtr->ForceValue = (float)dataListPtr[i]["ForceValue"].GetDouble();  //                     (float)cJSON_GetObjectItem(dataPtr, "ForceValue")->valuedouble;
				p2dLauncherDataPtr->Resistance = (float)dataListPtr[i]["Resistance"].GetDouble();      // (float)cJSON_GetObjectItem(dataPtr, "Resistance")->valuedouble;

				p2dLauncherDataPtr->UnitMinColor.SetR((float)dataListPtr[i]["UnitMinColor"][0].GetDouble());                     //  (float)cJSON_GetArrayItem(tempArrPtr1, 0)->valuedouble;
				p2dLauncherDataPtr->UnitMinColor.SetG((float)dataListPtr[i]["UnitMinColor"][1].GetDouble());
				p2dLauncherDataPtr->UnitMinColor.SetB((float)dataListPtr[i]["UnitMinColor"][2].GetDouble());
				p2dLauncherDataPtr->UnitMinColor.SetA((float)dataListPtr[i]["UnitMinColor"][3].GetDouble());

				p2dLauncherDataPtr->UnitMaxColor.SetR((float)dataListPtr[i]["UnitMaxColor"][0].GetDouble());
				p2dLauncherDataPtr->UnitMaxColor.SetG((float)dataListPtr[i]["UnitMaxColor"][1].GetDouble());
				p2dLauncherDataPtr->UnitMaxColor.SetB((float)dataListPtr[i]["UnitMaxColor"][2].GetDouble());
				p2dLauncherDataPtr->UnitMaxColor.SetA((float)dataListPtr[i]["UnitMaxColor"][3].GetDouble());
				p2dLauncherDataPtr->UnitColorRandom = dataListPtr[i]["UnitColorRandom"].GetBool();                   // cJSON_GetObjectItem(dataPtr, "UnitColorRandom")->valueint != 0;
																													 // tempArrPtr1 = cJSON_GetObjectItem(dataPtr, "UnitColor");
				p2dLauncherDataPtr->UnitColor.SetR((float)dataListPtr[i]["UnitColor"][0].GetDouble());
				p2dLauncherDataPtr->UnitColor.SetG((float)dataListPtr[i]["UnitColor"][1].GetDouble());
				p2dLauncherDataPtr->UnitColor.SetB((float)dataListPtr[i]["UnitColor"][2].GetDouble());
				p2dLauncherDataPtr->UnitColor.SetA((float)dataListPtr[i]["UnitColor"][3].GetDouble());
				p2dLauncherDataPtr->UseColorCurve = dataListPtr[i]["UseColorCurve"].GetBool();        //  cJSON_GetObjectItem(dataPtr, "UseColorCurve")->valueint != 0;
				if (p2dLauncherDataPtr->UseColorCurve)
				{
					Value& colorKeyFrameListPtr = dataListPtr[i]["ColorKeyFrameList"];
					if (colorKeyFrameListPtr.IsArray())
					{
						for (int j = 0; j < colorKeyFrameListPtr.Size(); j++)
						{
							FrameColorPair* frameColorPair = new FrameColorPair;
							frameColorPair->Key = (float)colorKeyFrameListPtr[j]["Key"].GetDouble(); //float)cJSON_GetObjectItem(tempObjPtr1, "Key")->valuedouble;
							frameColorPair->Color.SetR((float)colorKeyFrameListPtr[j]["Value"][0].GetDouble());
							frameColorPair->Color.SetG((float)colorKeyFrameListPtr[j]["Value"][1].GetDouble());
							frameColorPair->Color.SetB((float)colorKeyFrameListPtr[j]["Value"][2].GetDouble());
							frameColorPair->Color.SetA((float)colorKeyFrameListPtr[j]["Value"][3].GetDouble());
							p2dLauncherDataPtr->ColorKeyFrameArray.push_back(frameColorPair);
						}
					}
				}

				p2dLauncherDataPtr->UseRotation = dataListPtr[i]["UseRotation"].GetBool();          // cJSON_GetObjectItem(dataPtr, "UseRotation")->valueint != 0;
				p2dLauncherDataPtr->UnitRotation = (float)dataListPtr[i]["UnitRotation"].GetDouble();           // (float)cJSON_GetObjectItem(dataPtr, "UnitRotation")->valuedouble;
				p2dLauncherDataPtr->UnitMinRotation = (float)dataListPtr[i]["UnitMinRotation"].GetDouble();                   // (float)cJSON_GetObjectItem(dataPtr, "UnitMinRotation")->valuedouble;
				p2dLauncherDataPtr->UnitMaxRotation = (float)dataListPtr[i]["UnitMaxRotation"].GetDouble();       // (float)cJSON_GetObjectItem(dataPtr, "UnitMaxRotation")->valuedouble;
				p2dLauncherDataPtr->UnitRotationRandom = dataListPtr[i]["UnitRotationRandom"].GetBool();                    // cJSON_GetObjectItem(dataPtr, "UnitRotationRandom")->valueint != 0;
				p2dLauncherDataPtr->UseAddRotation = dataListPtr[i]["UseAddRotation"].GetBool();        // cJSON_GetObjectItem(dataPtr, "UseAddRotation")->valueint != 0;
				p2dLauncherDataPtr->UnitAddRotation = (float)dataListPtr[i]["UnitAddRotation"].GetDouble();       //                  (float)cJSON_GetObjectItem(dataPtr, "UnitAddRotation")->valuedouble;
				p2dLauncherDataPtr->UnitMinAddRotation = (float)dataListPtr[i]["UnitMinAddRotation"].GetDouble();                                                // (float)cJSON_GetObjectItem(dataPtr, "UnitMinAddRotation")->valuedouble;
				p2dLauncherDataPtr->UnitMaxAddRotation = (float)dataListPtr[i]["UnitMaxAddRotation"].GetDouble();                            //(float)cJSON_GetObjectItem(dataPtr, "UnitMaxAddRotation")->valuedouble;
				p2dLauncherDataPtr->UnitAddRotationRandom = dataListPtr[i]["UnitAddRotationRandom"].GetBool();                     // cJSON_GetObjectItem(dataPtr, "UnitAddRotationRandom")->valueint != 0;

				p2dLauncherDataPtr->UnitMinSize = (float)dataListPtr[i]["UnitMinSize"].GetDouble();   // (float)cJSON_GetObjectItem(dataPtr, "UnitMinSize")->valuedouble;
				p2dLauncherDataPtr->UnitMaxSize = (float)dataListPtr[i]["UnitMaxSize"].GetDouble();  // (float)cJSON_GetObjectItem(dataPtr, "UnitMaxSize")->valuedouble;
				p2dLauncherDataPtr->UnitSizeRandom = dataListPtr[i]["UnitSizeRandom"].GetBool();      //               cJSON_GetObjectItem(dataPtr, "UnitSizeRandom")->valueint != 0;
				p2dLauncherDataPtr->UnitSizeX = (float)dataListPtr[i]["UnitSizeX"].GetDouble();     // (float)cJSON_GetObjectItem(dataPtr, "UnitSizeX")->valuedouble;
				p2dLauncherDataPtr->UnitSizeY = (float)dataListPtr[i]["UnitSizeY"].GetDouble();   // (float)cJSON_GetObjectItem(dataPtr, "UnitSizeY")->valuedouble;
				p2dLauncherDataPtr->UseLifeTimeSizeChange = dataListPtr[i]["UseLifeTimeSizeChange"].GetBool();             // cJSON_GetObjectItem(dataPtr, "UseLifeTimeSizeChange")->valueint != 0;
				p2dLauncherDataPtr->UnitLifetimeStartSize = (float)dataListPtr[i]["UnitLifetimeStartSize"].GetDouble();                                 // (float)cJSON_GetObjectItem(dataPtr, "UnitLifetimeStartSize")->valuedouble;
				p2dLauncherDataPtr->UnitLifetimeFinSize = (float)dataListPtr[i]["UnitLifetimeFinSize"].GetDouble();  // (float)cJSON_GetObjectItem(dataPtr, "UnitLifetimeFinSize")->valuedouble;

				p2dLauncherDataPtr->UnitFrameColumnNum = dataListPtr[i]["UnitFrameColumnNum"].GetInt();             // cJSON_GetObjectItem(dataPtr, "UnitFrameColumnNum")->valueint;
				p2dLauncherDataPtr->UnitFrameRowNum = dataListPtr[i]["UnitFrameRowNum"].GetInt();          // cJSON_GetObjectItem(dataPtr, "UnitFrameRowNum")->valueint;
				p2dLauncherDataPtr->UnitFrameFPS = dataListPtr[i]["UnitFrameFPS"].GetInt();            // cJSON_GetObjectItem(dataPtr, "UnitFrameFPS")->valueint;
				p2dLauncherDataPtr->UnitFrameRandom = dataListPtr[i]["UnitFrameRandom"].GetBool();               // cJSON_GetObjectItem(dataPtr, "UnitFrameRandom")->valueint != 0;
				p2dLauncherDataPtr->UseFrame = dataListPtr[i]["UseFrame"].GetBool();                   // cJSON_GetObjectItem(dataPtr, "UseFrame")->valueint != 0;

				p2dLauncherDataPtr->UseUVScroll = dataListPtr[i]["UseUVScroll"].GetBool();          // cJSON_GetObjectItem(dataPtr, "UseUVScroll")->valueint != 0;
				p2dLauncherDataPtr->XScrollSpeed = (float)dataListPtr[i]["XScrollSpeed"].GetDouble();// (float)cJSON_GetObjectItem(dataPtr, "XScrollSpeed")->valuedouble;
				p2dLauncherDataPtr->YScrollSpeed = (float)dataListPtr[i]["YScrollSpeed"].GetDouble();        // (float)cJSON_GetObjectItem(dataPtr, "YScrollSpeed")->valuedouble;

				p2dLauncherDataPtr->ShaderType = dataListPtr[i]["ShaderType"].GetString();           // cJSON_GetObjectItem(dataPtr, "ShaderType")->valuestring;
				p2dLauncherDataPtr->ImageName = dataListPtr[i]["ImageName"].GetString();              //cJSON_GetObjectItem(dataPtr, "ImageName")->valuestring;

				assetData->m_desc.LauncherData.push_back(p2dLauncherDataPtr);
			}
		}
		return true;
	}


}