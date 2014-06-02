#include "ofxPrint.h"

namespace ofx
{
	namespace mtl
	{
		//--------------------------------------------------------------
		PrintEventArgs::PrintEventArgs(const string& printerName, float duration, bool bSuccess)
			: printerName(printerName)
			, duration(duration)
			, bSuccess(bSuccess)
		{

		}

		//--------------------------------------------------------------
		Print::Print()
			: ofThread()
			, _name("")
			, _path("")
			, _bNeedsNotify(false)
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
		}

		//--------------------------------------------------------------
		void Print::setPrinterName(const string& name)
		{
			_name = name;
			_args.printerName = name;
		}
		
		//--------------------------------------------------------------
		const string& Print::getPrinterName()
		{
			return _name;
		}

		//--------------------------------------------------------------
		void Print::printImage(const string& path, bool bThreaded)
		{
			_mode = OFX_PRINT_IMAGE_PATH;
			_path = path;

			_bNeedsNotify = false;

			if (bThreaded) {
				startThread(false);
			}
			else {
				threadedFunction();
			}
		}

		//--------------------------------------------------------------
		void Print::threadedFunction()
		{
			bool bSuccess = true;

#ifndef TARGET_WIN32
			ofLogError("ofxPrint") << "Only implemented for Windows so far, sorry!";
			bSuccess = false;
#endif
			if (_name == "") {
				ofLogError("ofxPrint") << "Printer name has not been set!";
				bSuccess = false;
			}

			if (_path == "") {
				ofLogError("ofxPrint") << "File path has not been set!";
				bSuccess = false;
			}

			if (!ofFile::doesFileExist(_path)) {
				ofLogError("ofxPrint") << "No file found at path " << _path;
				bSuccess = false;
			}

			if (_mode != OFX_PRINT_IMAGE_PATH) {
				ofLogError("ofxPrint") << "Unrecognized PrintMode " << _mode;
				bSuccess = false;
			}

			if (bSuccess) {
				float startTime = ofGetElapsedTimef();
			
				string cmdStr = "";
				cmdStr += "rundll32.exe \"C:\\Windows\\System32\\shimgvw.dll\", ImageView_PrintTo ";
				cmdStr += "\"" + _path + "\"";
				cmdStr += " ";
				cmdStr += "\"" + _name + "\"";
				ofSystem(cmdStr);

				_args.duration = ofGetElapsedTimef() - startTime;
			}
			else {
				_args.duration = -1;
			}

			_args.bSuccess = bSuccess;

			// Notify on the main thread.
			_bNeedsNotify = true;
		}

		//--------------------------------------------------------------
		void Print::update(ofEventArgs& args)
		{
			if (_bNeedsNotify) {
				ofNotifyEvent(printCompleted, _args, this);
				_bNeedsNotify = false;
			}
		}
	}
}
