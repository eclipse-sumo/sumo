#ifndef PHEMlightHELPERS
#define PHEMlightHELPERS

#include <string>


namespace PHEMlightdll {
    class Helpers {
        //--------------------------------------------------------------------------------------------------
        // Members 
        //--------------------------------------------------------------------------------------------------
    private:
        std::string _vClass;
    public:
        const std::string&  getvClass() const;
        void setvClass(const std::string& value);
    private:
        std::string _eClass;
    public:
        const std::string&  geteClass() const;
        void seteClass(const std::string& value);
    private:
        std::string _fClass;
    public:
        const std::string&  getfClass() const;
        void setfClass(const std::string& value);
    private:
        std::string _sClass;
    public:
        const std::string&  getsClass() const;
        void setsClass(const std::string& value);
    private:
        std::string _pClass;
    public:
        const std::string&  getpClass() const;
        void setpClass(const std::string& value);
    private:
        std::string _Class;
    public:
        const std::string&  getgClass() const;
        void setgClass(const std::string& value);
    private:
        std::string _ErrMsg;
    public:
        const std::string&  getErrMsg() const;
        void setErrMsg(const std::string& value);
    private:
        std::string _commentPrefix;
    public:
        const std::string&  getCommentPrefix() const;
        void setCommentPrefix(const std::string& value);
    private:
        std::string _PHEMDataV;
    public:
        const std::string&  getPHEMDataV() const;
        void setPHEMDataV(const std::string& value);

        //Get vehicle class
    private:
        bool getvclass(const std::string& VEH);

        //Get fuel Class
        bool getfclass(const std::string& VEH);

        //Get propulsion class
        bool getpclass(const std::string& VEH);

        //Get size class
        bool getsclass(const std::string& VEH);

        //Get euro class
        bool geteclass(const std::string& VEH);

        //Set complet class string
    public:
        bool setclass(const std::string& VEH);


    };
}


#endif	//#ifndef PHEMlightHELPERS
