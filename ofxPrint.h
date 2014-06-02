#pragma once

#include "ofMain.h"

namespace ofx
{
	namespace mtl
	{
		//--------------------------------------------------------------
		class PrintEventArgs 
			: public ofEventArgs
		{
		public:
			PrintEventArgs(const string& printerName = "", float duration = -1, bool bSuccess = false);

			string printerName;
			float duration;
			bool bSuccess;
		};

		//--------------------------------------------------------------
		enum PrintMode
		{
			OFX_PRINT_IMAGE_PATH = 0,

			OFX_PRINT_NUM_MODES
		};

		//--------------------------------------------------------------
		class Print
			: public ofThread
		{
		public:
			Print();
			~Print();

			void setup(const string& name);

			void setPrinterName(const string& name);
			const string& getPrinterName();

			void printImage(const string& path, bool bThreaded = true);

			void threadedFunction();

			void update(ofEventArgs& args);

			ofEvent<PrintEventArgs> printCompleted;

		private:
			string _name;
			string _path;
			PrintMode _mode;

			PrintEventArgs _args;
			bool _bNeedsNotify;
		};
	}
}

typedef ofx::mtl::PrintEventArgs ofxPrintEventArgs;
typedef ofx::mtl::PrintMode ofxPrintMode;
typedef ofx::mtl::Print ofxPrint;
