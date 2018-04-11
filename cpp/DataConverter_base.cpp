/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of REDHAWK Basic Components DataConverter.
 *
 * REDHAWK Basic Components DataConverter is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * REDHAWK Basic Components DataConverter is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "DataConverter_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/

DataConverter_base::DataConverter_base(const char *uuid, const char *label) :
    Component(uuid, label),
    ThreadedComponent()
{
    loadProperties();

    dataChar_in = new bulkio::InCharPort("dataChar_in");
    addPort("dataChar_in", "Char input port for data.", dataChar_in);
    dataOctet_in = new bulkio::InOctetPort("dataOctet_in");
    addPort("dataOctet_in", "Octet input port for data.", dataOctet_in);
    dataShort_in = new bulkio::InShortPort("dataShort_in");
    addPort("dataShort_in", "Short input port for data.", dataShort_in);
    dataUshort_in = new bulkio::InUShortPort("dataUshort_in");
    addPort("dataUshort_in", "Unsigned Short input port for data.", dataUshort_in);
    dataFloat_in = new bulkio::InFloatPort("dataFloat_in");
    addPort("dataFloat_in", "Float input port for data.", dataFloat_in);
    dataDouble_in = new bulkio::InDoublePort("dataDouble_in");
    addPort("dataDouble_in", "Double input port for data.", dataDouble_in);
    dataChar_out = new bulkio::OutCharPort("dataChar_out");
    addPort("dataChar_out", "Char output port for data.", dataChar_out);
    dataOctet_out = new bulkio::OutOctetPort("dataOctet_out");
    addPort("dataOctet_out", "Octet output port for data.", dataOctet_out);
    dataShort_out = new bulkio::OutShortPort("dataShort_out");
    addPort("dataShort_out", "Short output port for data.", dataShort_out);
    dataUshort_out = new bulkio::OutUShortPort("dataUshort_out");
    addPort("dataUshort_out", "Unsigned Short output port for data.", dataUshort_out);
    dataFloat_out = new bulkio::OutFloatPort("dataFloat_out");
    addPort("dataFloat_out", "Float output port for data.", dataFloat_out);
    dataDouble_out = new bulkio::OutDoublePort("dataDouble_out");
    addPort("dataDouble_out", "Double output port for data.", dataDouble_out);
}

DataConverter_base::~DataConverter_base()
{
    delete dataChar_in;
    dataChar_in = 0;
    delete dataOctet_in;
    dataOctet_in = 0;
    delete dataShort_in;
    dataShort_in = 0;
    delete dataUshort_in;
    dataUshort_in = 0;
    delete dataFloat_in;
    dataFloat_in = 0;
    delete dataDouble_in;
    dataDouble_in = 0;
    delete dataChar_out;
    dataChar_out = 0;
    delete dataOctet_out;
    dataOctet_out = 0;
    delete dataShort_out;
    dataShort_out = 0;
    delete dataUshort_out;
    dataUshort_out = 0;
    delete dataFloat_out;
    dataFloat_out = 0;
    delete dataDouble_out;
    dataDouble_out = 0;
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void DataConverter_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    Component::start();
    ThreadedComponent::startThread();
}

void DataConverter_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    Component::stop();
    if (!ThreadedComponent::stopThread()) {
        throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
    }
}

void DataConverter_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    // This function clears the component running condition so main shuts down everything
    try {
        stop();
    } catch (CF::Resource::StopError& ex) {
        // TODO - this should probably be logged instead of ignored
    }

    Component::releaseObject();
}

void DataConverter_base::loadProperties()
{
    addProperty(outputType,
                0,
                "outputType",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(maxTransferSize,
                0,
                "maxTransferSize",
                "",
                "readwrite",
                "samples",
                "external",
                "configure");

    addProperty(scaleOutput,
                scaleOutput_struct(),
                "scaleOutput",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(normalize_floating_point,
                normalize_floating_point_struct(),
                "normalize_floating_point",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(floatingPointRange,
                floatingPointRange_struct(),
                "floatingPointRange",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(advancedSRI,
                advancedSRI_struct(),
                "advancedSRI",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(transformProperties,
                transformProperties_struct(),
                "transformProperties",
                "",
                "readwrite",
                "",
                "external",
                "configure");

}


