
#include "viewporttool.h"
#include "userIO.h"



class landmark_tool : public nav_tool
{
	protected:
		static int userIO_ID;
//		static int point_collection_ID;	
		
	public:
		landmark_tool(viewport_event &);
		~landmark_tool();
		
		static const std::string name();
		static void init();
		virtual void handle(viewport_event &);
		
		static void register_userIO_ID(int ID);
//		static void register_point_collection_ID(int ID);
};
