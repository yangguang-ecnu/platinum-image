//$Id$

#include "bruker.h"

#include "fileutils.h"
#include "image_scalar.hxx"
#include "rawimporter.h"

//#include <iostream>

brukerloader::brukerloader(std::vector<std::string> * f): imageloader(f)
{
    //determine level (session, run, reconstruction)
    std::string parent = path_parent (files->front());
    
    if (file_exists (parent + "subject"))
        {
        //session level
        
        session = parent;
        
        runs = subdirs (session);
     
        std::vector<std::string>::iterator run = runs.begin();
        
        while (run != runs.end())
            {
            get_reconstructions (*run);
            ++run;
            }
        }
    
    else if (file_exists (parent + "acqp"))
        {
        //run level
        
        runs.push_back(parent);
        get_reconstructions(parent);
        }
    
    else if (file_exists (parent + "2dseq"))
        {
        //reconstruction level
        
        reconstructions.push_back(parent);
        }
}

void brukerloader::get_reconstructions(std::string run_dir_path)
{
    std::vector<std::string> A = subdirs (run_dir_path + "pdata");
    
    reconstructions.insert(reconstructions.end(),A.begin(),A.end());
}

class brukertoken //! helper that reads one Bruker metadata token
    {
    protected:
        std::string header_;
        std::vector <std::string> values;
        int level;

        const static int maxTokenSize = 512;

    public:
        brukertoken ()
            {
            level=0;
            }

        friend  std::istream& operator>>(std::istream&, brukertoken&);

        bool operator==(const brukertoken &k)
            { return (header_==k.header_ && level == k.level); }
        bool operator!=(const brukertoken &k)
            { return (header_!=k.header_ || level != k.level); }

        bool operator==(const std::string &k)
            { return header_==k; }
        bool operator!=(const std::string &k)
            { return header_!=k; }

        bool valid (std::string type = "")
            {
            return ((type == "" || header_ == type) && values.size() > 0 );
            }

        template <typename outType>
            outType value (int index = 0)
            {
                std::istringstream s(values[index]);
            outType result;
            
            s >> result;

            return result;
            }

        std::string header ()
            { return header_; }
    };

template <>
std::string brukertoken::value (int index)
    {
        return values[index];
    }

std::istream& operator>>(std::istream& in, brukertoken& o)
    {
    int numValues = 1;

    in.ignore (brukertoken::maxTokenSize,'#'); //locate next token

    getline (in,o.header_,'=');

    //count no of #s = indentation level
    while (o.header_.at(o.level) == '#')
        {
        o.level++;
        o.header_.erase(o.header_.begin());
        };

    if (o.header_.at(0) == '$')   //skip leading $ in header
        { o.header_.erase(o.header_.begin()); }


    //check for multi-value
    if ( in.peek() == '(' )
        {
        in.ignore (1);

        do {
            int n = 0;

            in >> n;
            numValues = numValues * n;
            }
    while (in.get() == ','); //multidimensional value num, multiply to produce total num of values

        in.ignore (3,'\n'); //skip to values at line below

        if (in.peek() == '<') //for lines enclosed with '<', ignore (incorrect) number of values information
            {
            numValues = 1;
            }

        }
    else
        {numValues = 1;}

    for (int i = 0; i < numValues; i++)
        {
        std::string v;

        if (numValues > 1)
            {
            in >> v;
            }
        else
            {
            //for single value tokens, value may contain spaces - get entire line
            getline (in,v,'\n');
            }

            o.values.push_back(std::string(v));
        }

    return in;
    }

image_base * brukerloader::read()
    {
    image_base * newImage = NULL;

    if (! reconstructions.empty())
        {
        std::vector<std::string>::iterator   reconstruction = reconstructions.begin();

        //open metadata files
        std::string rname = *reconstruction + "reco";
        std::string d3name = *reconstruction + "d3proc";

        std::ifstream reco (std::string(*reconstruction + "reco").c_str(),std::ios::in);
        std::ifstream d3proc (d3name.c_str(),std::ios::in);

        if (reco.is_open() && d3proc.is_open())
            {
            short size[3];
            Vector3D voxelsize;
            bool bigEndian;
            bool isSigned;
            bool isFloat;
            int bitDepth;

            // parse metadata tokens, store in map
            // metadata interpretations from
            // http://www.sph.sc.edu/comd/rorden/bru2anz.html

            std::map <std::string, brukertoken> tokens;

            bool goOn;
            do
                { 
                brukertoken t;

                reco >> t;

                tokens[t.header()] = t;

                goOn = t.valid() && !t.valid("END");
                }
            while (goOn);

            do
                { 
                brukertoken t;

                d3proc >> t;

                tokens[t.header()] = t;

                goOn = t.valid() && !t.valid("END");
                }
            while (goOn);

            reco.close();
            d3proc.close();

            //retrieve metadata of interest
            size [0] = tokens ["IM_SIX"].value<int>();
            size [1] = tokens ["IM_SIY"].value<int>();
            size [2] = tokens ["IM_SIZ"].value<int>();
 
			//bigEndian = true; //! according to site above, format is always bigEndian regardless of this value !!!!! Seems like it is always littleEndian //PR 070605
            bigEndian =tokens ["RECO_byte_order"].value<std::string>() != "littleEndian";

            //parse the wordtype string
            std::istringstream byteOrderString (tokens ["RECO_wordtype"].value<std::string>());

            byteOrderString.ignore(1);

            byteOrderString >> bitDepth ;

            byteOrderString.ignore(8,'_');

            std::string signedString;
            getline(byteOrderString,signedString,'_');
            isSigned = (signedString == "SGN");

            std::string typeString;
            getline(byteOrderString,typeString);
            isFloat = (typeString != "INT");
            //assumption: value is either "INT" or something to the amount of float

            //TODO: "RECO_rotate"
            
            //TODO: figure out where voxel size is set and read it

            voxelsize.Fill (1);

			if(isFloat)
				{
				newImage = allocate_image<image_scalar> (
					isFloat,
					isSigned,
					bitDepth,
					std::vector<std::string > (1,*reconstruction + "2dseq"), 
					size[0], size[1],
					bigEndian,
					0, 
					voxelsize);
				}
			else
				{
				newImage = allocate_image<image_integer> (
					isFloat,
					isSigned,
					bitDepth,
					std::vector<std::string > (1,*reconstruction + "2dseq"), 
					size[0], size[1],
					bigEndian,
					0, 
					voxelsize);
				}
            newImage->name( tokens ["OWNER"].value<std::string>() + tokens ["RECO_time"].value<std::string>());

            files->clear(); //eat all files if (one) load was successful
            reconstructions.erase(reconstruction);
            }
        }

    return newImage;
    }