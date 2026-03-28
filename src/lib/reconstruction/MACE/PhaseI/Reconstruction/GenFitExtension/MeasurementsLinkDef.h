#ifndef MEASUREMENTS_LINKDEF_H
#define MEASUREMENTS_LINKDEF_H

#ifdef __ROOTCLING__


#    pragma link off all globals;
#    pragma link off all classes;
#    pragma link off all functions;

#    pragma link C++ namespace genfit;
#    pragma link C++ class genfit::HelixMeasurement + ;

#endif

#endif
