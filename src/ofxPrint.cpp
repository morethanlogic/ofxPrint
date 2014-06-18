#include "ofxPrint.h"

namespace ofx
{
	namespace mtl
	{
		//--------------------------------------------------------------
		PrintEventArgs::PrintEventArgs(const string& printerName, float duration)
			: printerName(printerName)
			, mode(OFX_PRINT_ERROR)
			, path("")
			, duration(duration)
			, bSuccess(false)
		{

		}

		//--------------------------------------------------------------
		Print::Print()
			: ofThread()
			, _name("")
		{
			ofAddListener(ofEvents().update, this, &Print::update);
		}

		//--------------------------------------------------------------
		Print::~Print()
		{
			ofRemoveListener(ofEvents().update, this, &Print::update);
		}

		//--------------------------------------------------------------
		void Print::setup(const string& name)
		{
			setPrinterName(name);
			startThread();
		}

		//--------------------------------------------------------------
		void Print::setPrinterName(const string& name)
		{
			_name = name;
		}
		
		//--------------------------------------------------------------
		const string& Print::getPrinterName()
		{
			return _name;
		}

		//--------------------------------------------------------------
		void Print::printImage(const string& path, bool bThreaded)
		{
			PrintEventArgs *args = new PrintEventArgs(_name);
			args->mode = OFX_PRINT_IMAGE_PATH;
			args->path = path;

			if (bThreaded) {
				lock();
				printQueue.push_back(args);
				unlock();
			}
			else {
				printImageImpl(args);

				lock();
				resultQueue.push_back(args);
				unlock();
			}
		}

		//--------------------------------------------------------------
		void Print::printImage(const ofImage& image, bool bThreaded)
		{
			// TODO
		}

		//--------------------------------------------------------------
		void Print::threadedFunction()
		{
			while (isThreadRunning()) {
				if (!printQueue.empty()) {
					// Print the item at the top of the queue.
					PrintEventArgs *args = printQueue.front();
					printImageImpl(args);

					// Move the args from the print to the result queue, for notification on the main thread.
					lock();
					printQueue.pop_front();
					resultQueue.push_back(args);
					unlock();
				}
			}
		}

		//--------------------------------------------------------------
		void Print::update(ofEventArgs& args)
		{
			if (!resultQueue.empty()) {
				if (mutex.tryLock()) {
					// Notify the item at the top of the queue.
					PrintEventArgs *args = resultQueue.front();
					ofNotifyEvent(printCompleted, *args, this);

					// Remove and delete the item.
					resultQueue.pop_front();
					delete args;

					unlock();
				}
			}
		}

		//--------------------------------------------------------------
		void Print::printImageImpl(PrintEventArgs *args)
		{
			bool bSuccess = true;

#ifndef TARGET_WIN32
			ofLogError("ofxPrint") << "Only implemented for Windows so far, sorry!";
			bSuccess = false;
#endif
			if (args->printerName == "") {
				ofLogError("ofxPrint") << "Printer name has not been set!";
				bSuccess = false;
			}

			if (args->mode == OFX_PRINT_ERROR) {
				ofLogError("ofxPrint") << "PrintMode has not been set!";
				bSuccess = false;
			}
			else if (args->mode == OFX_PRINT_IMAGE_PATH) {
				if (args->path == "") {
					ofLogError("ofxPrint") << "File path has not been set!";
					bSuccess = false;
				}

				if (!ofFile::doesFileExist(args->path)) {
					ofLogError("ofxPrint") << "No file found at path " << args->path;
					bSuccess = false;
				}
			}
			else if (args->mode == OFX_PRINT_IMAGE_OF) {
				// TODO
			}
			else {
				ofLogError("ofxPrint") << "Unrecognized PrintMode " << args->mode;
				bSuccess = false;
			}

			if (bSuccess) {
				float startTime = ofGetElapsedTimef();
			
				string cmdStr = "";
				cmdStr += "rundll32.exe \"C:\\Windows\\System32\\shimgvw.dll\", ImageView_PrintTo ";
				cmdStr += "\"" + args->path + "\"";
				cmdStr += " ";
				cmdStr += "\"" + args->printerName + "\"";
				ofSystem(cmdStr);

				args->duration = ofGetElapsedTimef() - startTime;
			}
			else {
				args->duration = -1;
			}

			args->bSuccess = bSuccess;
			
			ofLogVerbose("ofxPrint") << "Printed image:\n\tpath = " << args->path << "\n\tduration = " << args->duration << "\n\tbSuccess = " << args->bSuccess;
		}
	}
}
