#!/usr/bin/env python
#
# This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
# source distribution.
# 
# This file is part of REDHAWK Basic Components DataConverter.
# 
# REDHAWK Basic Components DataConverter is free software: you can redistribute it and/or modify it under the terms of 
# the GNU General Public License as published by the Free Software Foundation, either 
# version 3 of the License, or (at your option) any later version.
# 
# REDHAWK Basic Components DataConverter is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
# PURPOSE.  See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along with this 
# program.  If not, see http://www.gnu.org/licenses/.
#

import unittest
import ossie.utils.testing
import os
from omniORB import any
from ossie.utils.bulkio import bulkio_data_helpers
import sys
import time
import threading
from new import classobj
from omniORB import any, CORBA
from bulkio.bulkioInterfaces import BULKIO, BULKIO__POA
import bulkio
from ossie.cf import CF, CF__POA
from ossie.utils import uuid
#from ossie.resource import Resouce
from ossie.properties import simple_property
import subprocess
from ossie.utils import sb
import struct, math
import pickle
from scipy import signal
import numpy as np
#import matplotlib.pyplot as plt
from scipy import stats
import random as rand

import cmath,math
from array import array
from fractions import Fraction

import scipy
#Scipy changed the api for correlate in version 0.8
from distutils.version import LooseVersion
SCIPY_GREATER_08 = LooseVersion(scipy.__version__) >= LooseVersion('0.8')
_orig_correlate = scipy.signal.correlate
def correlatewrap(data,filter,*args, **kwargs):
    if len(filter) > len(data):
        return _orig_correlate(filter,np.conj(data),*args, **kwargs)    
    else:
        return _orig_correlate(data,np.conj(filter),*args, **kwargs)
if SCIPY_GREATER_08:
    scipy.signal.correlate = correlatewrap


class ComponentTests(ossie.utils.testing.ScaComponentTestCase):
    """Test for all component implementations in DataConverter"""

    def setUp(self):
        ossie.utils.testing.ScaComponentTestCase.setUp(self)
        self.length = 32
        #self.initializeDicts()

    def initializeDicts(self):
        self.types = ['Char', 'Octet', 'Short', 'Ushort', 'Float', 'Double']
        self.inputPorts = {}
        self.outputPorts = {}
        self.portTypes = {}
        self.inputFiles = {}
        for type in self.types:
            self.inputPorts[type] = self.comp_obj.getPort('data' + str(type) + '_in')
            self.outputPorts[type] = self.comp.getPort('data' + str(type) + '_out')
            self.portTypes[type] = 'BULKIO__POA.data' + str(type)
            self.inputFiles[type] = str(type) + '.dat'
        self.sri = BULKIO.StreamSRI(1,0.0,0.001,1,200,0.0,0.001,1,1,"dataConvTest", 0 , [])
        self.sri.xdelta = 1.0/400
        self.sri.mode = 0
        self.length = 32

    def scaleInputPort(self,scale,type):
        if type == "float":
            self.comp.normalize_floating_point.Input = scale
        elif type == "double":
            self.comp.normalize_floating_point.Input = scale

    def scaleOutputPort(self,scale,type):
        if type == "char":
            self.comp.scaleOutput.charPort = scale
        elif type == "octet":
            self.comp.scaleOutput.octetPort = scale
        elif type == "short":
            self.comp.scaleOutput.shortPort = scale
        elif type == "ushort":
            self.comp.scaleOutput.ushortPort = scale
        elif type == "float":
            self.comp.normalize_floating_point.output = scale
        elif type == "double":
            self.comp.normalize_floating_point.output = scale
    
    def getInputPortNames(self,type):
        if type == "char":
            return ('dataChar_in', 'charOut')
        elif type == "octet":
            return ('dataOctet_in','octetOut')
        elif type == "short":
            return ('dataShort_in', 'shortOut')
        elif type == "ushort":
            return ('dataUshort_in', 'ushortOut')
        elif type == "float":
            return ('dataFloat_in', 'floatOut')
        elif type == "double":
            return ('dataDouble_in', 'doubleOut')

    def getOutputPortNames(self,type):
        if type == "char":
            return ('charIn','dataChar_out')
        elif type == "octet":
            return ('octetIn','dataOctet_out')
        elif type == "short":
            return ('shortIn','dataShort_out')
        elif type == "ushort":
            return ('ushortIn','dataUshort_out')
        elif type == "float":
            return ('floatIn','dataFloat_out')
        elif type == "double":
            return ('doubleIn','dataDouble_out')

    def makeDataFiles(self,sType,sMin,sRange,dType,dMin,dRange,scale):        
        floatMin = -1 # data is generated as float data
        floatRange = 2
        
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        sint = np.sin(50000*t)
        
        if scale:
            # cast to input type
            inData = ((sint-floatMin)*float(Fraction(sRange,floatRange))+sMin).astype(sType)
            # convert to output type
            
            expectedData = ((np.float32(inData)-np.float32(sMin))*np.float32(Fraction(dRange,sRange))+dMin).astype(dType)
        else:
            # If the data is not scaled the just create some data and then convert it to the output type
            inData = (100*sint).astype(sType)
            expectedData = inData.astype(dType)
        
        return(inData,expectedData)
    
    def testTimeCode(self):
        
        class mySinkPort(bulkio.InShortPort):
            
            def __init__(self,portname):
                bulkio.InShortPort.__init__(self,portname)
                self.lastTimeCode = bulkio.timestamp.notSet()
            
            def pushPacket(self, data, T, EOS, streamID):
                #print 'mySinkPort "%s" pushPacket called with %s samples and %s tstamp (%s,%s)'%(self.name,len(data),T.twsec+T.tfsec, streamID, EOS)
                if not (EOS and T == bulkio.timestamp.notSet()):
                    self.lastTimeCode = T
                
        
        self.comp = sb.launch('../DataConverter.spd.xml')
        snk=mySinkPort("dataShort_in")
        outputPorts = self.getOutputPortNames("short")
        inputPorts = self.getInputPortNames("short")
        outport = self.comp.getPort(outputPorts[1])
        outport.connectPort(snk._this(),"connectionID")
        sb.start()
        
        input_port = self.comp.getPort(inputPorts[0])

        self.sri = bulkio.sri.create("dataConvTest")
        input_port.pushSRI(self.sri)
        wsec = 123456.0
        fsec = 0.654321
        data = range(1024)
        T = bulkio.timestamp.create(wsec, fsec)
        input_port.pushPacket(data,T,True,"dataConvTest") 
        time.sleep(.5)
           
        lastTimeCode = snk.lastTimeCode
        self.assertEqual(lastTimeCode.twsec,wsec)
        self.assertEqual(lastTimeCode.tfsec,fsec)

        # Test invalid timecode. A warning should be produced but the timecode is still passed on.
        wsec = 987654.0
        fsec = 0.456789
        data = range(1024)
        #T = BULKIO.PrecisionUTCTime(BULKIO.TCM_CPU, BULKIO.TCS_INVALID, 0, wsec, fsec)
        T = bulkio.timestamp.create(wsec, fsec)
        T.tcstatus = BULKIO.TCS_INVALID
        input_port.pushSRI(self.sri)
        input_port.pushPacket(data,T,True,"dataConvTest") 
        time.sleep(.5)
        
        lastTimeCode = snk.lastTimeCode
        self.assertEqual(lastTimeCode.twsec,wsec)
        self.assertEqual(lastTimeCode.tfsec,fsec)
        
        sb.stop()
        self.comp.releaseObject()
    
    def runtestcase(self,scale,inType,outType,inData,expectedData):

        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
     
        #set scaling properties
        self.scaleInputPort(scale,inType)
        self.scaleOutputPort(scale,outType)
        
        src=sb.StreamSource(format=inType)
        snk=sb.StreamSink()
        
        inputPorts = self.getInputPortNames(inType)
        outputPorts = self.getOutputPortNames(outType)
        src.connect(providesComponent=self.comp,providesPortName = inputPorts[0], usesPortName=inputPorts[1])
        self.comp.connect(providesComponent=snk,providesPortName=outputPorts[0],usesPortName=outputPorts[1])
        sb.start()
        src.complex=False
        twsec = math.floor(time.time())
        src.write(data=inData.tolist())
        src.close()
        time.sleep(1)
                
        streamdata = snk.read(timeout=5)     
        if not(streamdata):
              self.assertTrue(False, "No Data Stream Received")
        print "Got Data of length " , len(streamdata.data), len(inData) , len(expectedData)
        result = streamdata.data
        tstamps = streamdata.timestamps
        sb.stop()
        self.comp.releaseObject()
        count = 0;
        
        debug =0
        if debug:
            print inData
            print "&&&&&&&&&&&&&&&&&&&&&&&&&"
            print expectedData
            print "&&&&&&&&&&&&&&&&&&&&&&&&&"
            print result
        
        # since there is a fraction involved and rounding can be different, we will check for == =+1 or =-1        
        for i in range(0,len(result)):
            if (outType in ("float", "double")):
                self.assertTrue(abs(expectedData[i] - result[i]) < 0.00001)         
            elif (outType in ("octet")):
                self.assertTrue(expectedData[i]+1 == result[i] or expectedData[i]-1 == result[i] or expectedData[i] == result[i])                       
            else:
                self.assertTrue(expectedData[i]+1 == result[i] or expectedData[i]-1 == result[i] or expectedData[i] == result[i])    
        self.assertNotEqual(len(result), 0, "Did not receive pushed data!")

        
        self.assertEqual(tstamps[0][0],0) #First timestamp offset should be 0
        self.assertAlmostEqual(tstamps[0][1].twsec,twsec,1) #First timestamp should have start time when data was sent in 
       

    def char2char(self,scale=True):

        #data type info
        sType = np.int8 # input type (from)
        sMin = -128
        sRange = 255
        dType = np.int8 # output type (to)
        dMin = -128
        dRange = 255
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "char", "char",inData,expectedData)



    def char2octet(self,scale=True):

        #data type info
        sType = np.int8 # input type (from)
        sMin = -128
        sRange = 255
        dType = np.uint8 # output type (to)
        dMin = 0
        dRange = 255
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "char", "octet",inData,expectedData)        
     

    def char2short(self, scale=True):

        #data type info
        sType = np.int8 # input type (from)
        sMin = -128
        sRange = 255
        dType = np.int16 # output type (to)
        dMin = -32768
        dRange = 65535 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "char", "short",inData,expectedData)


    def char2ushort(self, scale=True):
    
        #data type info
        sType = np.int8 # input type (from)
        sMin = -128
        sRange = 255
        dType = np.uint16 # output type (to)
        dMin = 0
        dRange = 65535 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "char", "ushort",inData,expectedData)
     
        
    
    def char2float(self, scale=True):

         #data type info
        sType = np.int8 # input type (from)
        sMin = -128
        sRange = 255
        dType = np.float32 # output type (to)
        dMin = -1
        dRange = 2 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "char", "float",inData,expectedData)       

    
    def char2double(self, scale=True):

         #data type info
        sType = np.int8 # input type (from)
        sMin = -128
        sRange = 255
        dType = np.float64 # output type (to)
        dMin = -1
        dRange = 2 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "char", "double",inData,expectedData)    

    
    def octet2char(self,scale=True):
                
         #data type info
        sType = np.uint8 # input type (from)
        sMin = 0
        sRange = 255
        dType = np.int8 # output type (to)
        dMin = -128
        dRange = 255 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "octet", "char",inData,expectedData)    

    def octet2octet(self,scale=True):
        
         #data type info
        sType = np.uint8 # input type (from)
        sMin = 0
        sRange = 255
        dType = np.uint8 # output type (to)
        dMin = 0
        dRange = 255 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "octet", "octet",inData,expectedData)

    def octet2short(self, scale=True):
        
         #data type info
        sType = np.uint8 # input type (from)
        sMin = 0
        sRange = 255
        dType = np.int16 # output type (to)
        dMin = -32768
        dRange = 65535 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "octet", "short",inData,expectedData)
        

    def octet2ushort(self, scale=True):
         #data type info
        sType = np.uint8 # input type (from)
        sMin = 0
        sRange = 255
        dType = np.uint16 # output type (to)
        dMin = 0
        dRange = 65535 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "octet", "ushort",inData,expectedData)

    
    def octet2float(self, scale=True):
         #data type info
        sType = np.uint8 # input type (from)
        sMin = 0
        sRange = 255
        dType = np.float32 # output type (to)
        dMin = -1
        dRange = 2 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "octet", "float",inData,expectedData)

    
    def octet2double(self, scale=True):
         #data type info
        sType = np.uint8 # input type (from)
        sMin = 0
        sRange = 255
        dType = np.float64 # output type (to)
        dMin = -1
        dRange = 2 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "octet", "double",inData,expectedData)

    
    def short2char(self,scale=True):
         #data type info
        sType = np.int16 # input type (from)
        sMin = -32768
        sRange = 65535
        dType = np.int8 # output type (to)
        dMin = -128
        dRange = 255 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "short", "char",inData,expectedData)
        
    def short2octet(self,scale=True):
        #data type info
        sType = np.int16 # input type (from)
        sMin = -32768
        sRange = 65535
        dType = np.uint8 # output type (to)
        dMin = 0
        dRange = 255 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "short", "octet",inData,expectedData)

    def short2ushort(self, scale=True):
        #data type info
        sType = np.int16 # input type (from)
        sMin = -32768
        sRange = 65535
        dType = np.uint16 # output type (to)
        dMin = 0
        dRange = 65535 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "short", "ushort",inData,expectedData)


    def short2float(self, scale=True):
         #data type info
        sType = np.int16 # input type (from)
        sMin = -32768
        sRange = 65535
        dType = np.float32 # output type (to)
        dMin = -1
        dRange = 2 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "short", "float",inData,expectedData)       
   
    def short2double(self, scale=True):
        #data type info
        sType = np.int16 # input type (from)
        sMin = -32768
        sRange = 65535
        dType = np.float64 # output type (to)
        dMin = -1
        dRange = 2 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "short", "double",inData,expectedData) 
   
    def ushort2char(self,scale=True):
         #data type info
        sType = np.uint16 # input type (from)
        sMin = 0
        sRange = 65535
        dType = np.int8 # output type (to)
        dMin = -128
        dRange = 255 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "ushort", "char",inData,expectedData)                
        
    def ushort2octet(self,scale=True):
                #data type info
        sType = np.uint16 # input type (from)
        sMin = 0
        sRange = 65535
        dType = np.uint8 # output type (to)
        dMin = 0
        dRange = 255 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "ushort", "octet",inData,expectedData)
       
    def ushort2short(self, scale=True):
        #data type info
        sType = np.uint16 # input type (from)
        sMin = 0
        sRange = 65535
        dType = np.int16 # output type (to)
        dMin = -32768
        dRange = 65535 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "ushort", "short",inData,expectedData)       

    def ushort2float(self, scale=True):
         #data type info
        sType = np.uint16 # input type (from)
        sMin = 0
        sRange = 65535
        dType = np.float32 # output type (to)
        dMin = -1
        dRange = 2 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "ushort", "float",inData,expectedData)
       
    def ushort2double(self, scale=True):
        #data type info
        sType = np.uint16 # input type (from)
        sMin = 0
        sRange = 65535
        dType = np.float64 # output type (to)
        dMin = -1
        dRange = 2 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "ushort", "double",inData,expectedData)        
    def float2char(self,scale=True):
                       
        #data type info
        sType = np.float32 # input type (from)
        sMin = -1
        sRange = 2
        dType = np.int8 # output type (to)
        dMin = -128
        dRange = 255
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "float", "char",inData,expectedData)

    def float2octet(self,scale=True):
        
        #data type info
        sType = np.float32 # input type (from)
        sMin = -1
        sRange = 2
        dType = np.uint8 # output type (to)
        dMin = 0
        dRange = 255
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "float", "octet",inData,expectedData)

    def float2short(self,scale=True):
        #data type info
        sType = np.float32 # input type (from)
        sMin = -1
        sRange = 2
        dType = np.int16 # output type (to)
        dMin = -32768
        dRange = 65535  
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "float", "short",inData,expectedData)

    def float2ushort(self,scale=True):
        #data type info
        sType = np.float32 # input type (from)
        sMin = -1
        sRange = 2
        dType = np.uint16 # output type (to)
        dMin = 0
        dRange = 65535  
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "float", "ushort",inData,expectedData)
    
    def float2float(self,scale=True):
        
        #data type info
        sType = np.float32 # input type (from)
        sMin = -1
        sRange = 2
        dType = np.float32 # output type (to)
        dMin = -1
        dRange = 2
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "float", "float",inData,expectedData)

    def float2double(self,scale=True):
        
        #data type info
        sType = np.float32 # input type (from)
        sMin = -1
        sRange = 2
        dType = np.float64 # output type (to)
        dMin = -1
        dRange = 2
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "float", "double",inData,expectedData)
    
    def double2char(self,scale=True):

        #data type info
        sType = np.float64 # input type (from)
        sMin = -1
        sRange = 2
        dType = np.int8 # output type (to)
        dMin = -128
        dRange = 255 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "double", "char",inData,expectedData)
    
    def double2octet(self,scale=True):
        
        #data type info
        sType = np.float64 # input type (from)
        sMin = -1
        sRange = 2
        dType = np.uint8 # output type (to)
        dMin = 0
        dRange = 255 
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "double", "octet",inData,expectedData)

    def double2short(self,scale=True):
        #data type info
        sType = np.float64 # input type (from)
        sMin = -1
        sRange = 2
        dType = np.int16 # output type (to)
        dMin = -32768
        dRange = 65535
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "double", "short",inData,expectedData)
        
    
    def double2ushort(self,scale=True):
        #data type info
        sType = np.float64 # input type (from)
        sMin = -1
        sRange = 2
        dType = np.uint16 # output type (to)
        dMin = 0
        dRange = 65535
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "double", "ushort",inData,expectedData)
    
    def double2float(self,scale=True):
        #data type info
        sType = np.float64 # input type (from)
        sMin = -1
        sRange = 2
        dType = np.float32 # output type (to)
        dMin = -1
        dRange = 2
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "double", "float",inData,expectedData)

    def double2double(self,scale=True):
        #data type info
        sType = np.float64 # input type (from)
        sMin = -1
        sRange = 2
        dType = np.float64 # output type (to)
        dMin = -1
        dRange = 2
        
        inData, expectedData= self.makeDataFiles(sType,sMin,sRange,dType,dMin,dRange,scale)
        
        self.runtestcase(scale, "double", "float",inData,expectedData)

    def realToComplex(self, scale=True, enablePlt=False):

        FAIL_TEST=0
        #DEBUG_LEVEL = 5
        #LOG_LEVEL = CF.LogLevels.DEBUG
        LOG_LEVEL = CF.LogLevels.INFO
        

        self.comp = sb.launch('../DataConverter.spd.xml')
        #self.comp = sb.launch('../DataConverter.spd.xml',execparams={'DEBUG_LEVEL':DEBUG_LEVEL})
        #self.comp = sb.launch('../DataConverter.spd.xml',properties={'DEBUG_LEVEL':DEBUG_LEVEL})
        self.comp.log_level(LOG_LEVEL)
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        self.comp.start()
        
        #set properties we care about
        self.comp.transformProperties.fftSize = 2048
        self.comp.transformProperties.lowCutoff = .001
        self.comp.transformProperties.highCutoff = .499
        self.comp.transformProperties.transitionWidth = .001
        self.comp.transformProperties.tune_fs_over_4 = False
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        self.comp.outputType = 2
        
        #get a bunch of random data between 1 and -1 and create a BPSK Signal
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./32768)
        sint = np.sin(50000*t) * np.sin(1000*t)
        
        src=sb.DataSource()#bytesPerPush=512 + 32)
        snk=sb.DataSink()

        src.connect(self.comp, "dataFloat_in")
        self.comp.connect(snk, "floatIn")
        
        sb.start()
        src.push(data=sint.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        sb.stop()
        self.comp.log_level(CF.LogLevels.INFO)
        self.comp.releaseObject()
        self.assertNotEqual(len(result), 0, "Did not receive pushed data!")
        
        nans = [1 for x in sint if x != x]
        print 'len sint, len nans:', len(sint), len(nans)
        nans = [1 for x in result if x != x]
        print 'len result, len nans:', len(result), len(nans)

        res = np.array([np.complex(result[ii], result[ii+1]) for ii in np.arange(0,len(result)-1,2)])
        
        h=signal.hilbert(sint)
        hReal = h.real
        hImag = h.imag
        
        # Stats Section
        avgOut = np.mean(res)
        avgKnown = np.mean(h)
        stdDeviation = np.std(h)
        
        print 'avg res, avg h, stddev h:', avgOut, avgKnown, stdDeviation

        corr = signal.correlate(h,res)
        elementer = corr.argmax() # index of max correlation
        should = int((len(h) + len(res))/2 - 1) # average length of expected and actual output, minus 1
        
        print 'len h, len res, should, elementer, should-elementer, corr:',len(h), len(res), should, elementer, should-elementer, corr
        
        self.assertNotEqual(elementer, 0, 'No correlation found between expected and actual result!')

        error_real = np.zeros(min(len(h), len(res)))
        error_imag = np.zeros(min(len(h), len(res)))
        error_mag = np.zeros(min(len(h), len(res)))
        error = np.array([np.complex(error_real[ii], error_imag[ii]) for ii in np.arange(len(error_imag))])
        jj = 0
        # calculate absolute error relative to the standard deviation
        print 'ii, jj, error_mag, h.real, res.real, h.imag, res.imag, stddev'
        for ii in np.arange(should-elementer, len(h) + should-elementer):
            error[jj] = np.complex((h.real[jj] - res.real[ii]), (h.imag[jj] - res.imag[ii]))
            error_mag[jj] = np.abs(np.complex((h.real[jj] - res.real[ii]), (h.imag[jj] - res.imag[ii]))) / stdDeviation
            if jj%4096 == 0: print ii, jj, error_mag[jj], h.real[jj], res.real[ii], h.imag[jj], res.imag[ii], stdDeviation
            jj+=1
        
        
        
        if enablePlt: 
            ter = np.arange(should-elementer, len(h) + should-elementer)
            import matplotlib.pyplot as plt
            plt.figure(1)
            plt.subplot(411)
            plt.title('Original sin(t)')
            plt.plot(sint)
            plt.subplot(412)
            plt.title('Real Arm from Component sin(t)')
            plt.plot(h[0:len(ter)-1].real, 'r')
            plt.plot(res[ter].real, 'g')
            plt.subplot(413)
            plt.title('Imag Arm from Component sin(t)')
            plt.plot(h[0:len(ter)-1].imag, 'r')
            plt.plot(res[ter].imag, 'g')        
            plt.subplot(414)
            plt.title('error_mag')
            plt.plot(error_mag)            
            plt.show()
                
        print 'avg res - avg h =', avgOut-avgKnown
        print 'max(abs(error_mag)):', np.max(np.abs(error_mag))
        
        self.assertEqual((0.01>avgOut-avgKnown),True)
        self.assertEqual((0.1>np.max(np.abs(error_mag))),True)

        self.assertFalse(FAIL_TEST, "Test passed, failing assert for extra debug")


    def complexToReal(self, scale=True, enablePlt=False):

        FAIL_TEST=0
        #DEBUG_LEVEL = 5
        #LOG_LEVEL = CF.LogLevels.DEBUG
        LOG_LEVEL = CF.LogLevels.INFO
        

        self.comp = sb.launch('../DataConverter.spd.xml')
        #self.comp = sb.launch('../DataConverter.spd.xml',execparams={'DEBUG_LEVEL':DEBUG_LEVEL})
        #self.comp = sb.launch('../DataConverter.spd.xml',properties={'DEBUG_LEVEL':DEBUG_LEVEL})
        self.comp.log_level(LOG_LEVEL)
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        
        #set properties we care about
        self.comp.transformProperties.fftSize = 2048
        self.comp.transformProperties.lowCutoff = .001
        self.comp.transformProperties.highCutoff = .499
        self.comp.transformProperties.transitionWidth = .001
        self.comp.transformProperties.tune_fs_over_4 = False
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        self.comp.outputType=1
        
        #########################
        # Build input signal x
        #########################
        Fs = 1024
        secs = 16
        num = 50
        Fcs = stats.norm.rvs(loc=0, scale=Fs/4,size=num)
        Phases = np.array([rand.uniform(-np.pi, np.pi) for ii in np.arange(num)])

        T = np.linspace(0,secs,Fs*secs)
        To = np.linspace(0,secs,Fs*2*secs) # Only used for plotting
        
        #F = np.linspace(-Fs/2,Fs/2-1,Fs*secs)
        #Fo = np.linspace(-Fs,Fs-1,Fs*2*secs)
        
        x = np.zeros(len(T), dtype=np.complex)
        
        for ii in np.arange(len(Fcs)):
            x = x+np.exp(2j*np.pi*Fcs[ii]*T + 1j*Phases[ii])

        ############################
        # Compute output signal xo
        ############################
        # Force input to 32-bit floats, but perform calculations in double precision
        X = np.fft.fft(x.astype(np.complex64))
        X_FFT = np.fft.fftshift(X)
        #for ii in np.arange(len(X)/2,len(X)):
        #    X[ii]= X[ii]#.conjugate()
        X = np.fft.fftshift(X)
        Xo = np.zeros(len(X)*2,dtype=np.complex)
        Xo.put(np.arange(len(X)), X)
        Xo.put(np.arange(len(X)+1,len(X)*2), X[::-1].conjugate())
        Xo[0] = np.complex(Xo[0].real,0)
        # Cast result back to 32-bit floats after all calculations
        xo = np.fft.ifft(Xo).astype(np.complex64)
        
        ############################################
        # Push data through component to get result
        ############################################

        # for BulkIO, flatten x into Re{x(0)}, Im{x(0)}, Re{x(1)}, Im{x(1)}, ...
        # Also, force to 32-bit floats
        outer = np.zeros(len(x)*2, dtype=np.float32)
        for ii, val in enumerate(x.astype(np.complex64)):
            outer[2*ii] = val.real
            outer[2*ii+1] = val.imag

        # TODO Update to Source and Sink 
        src=sb.DataSource()#bytesPerPush=512+32)
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataFloat_in', usesPortName='floatOut')
        self.comp.connect(providesComponent=snk,providesPortName='floatIn',usesPortName='dataFloat_out')
        
        sb.start()
        src.push(data=outer.tolist(),sampleRate=Fs,EOS=True,complexData=True)
        time.sleep(1)
        (result,tstamps) = (snk.getData(tstamps=True))
        sb.stop()
        self.comp.log_level(CF.LogLevels.INFO)
        self.comp.releaseObject()
        self.assertNotEqual(len(result), 0, "Did not receive pushed data!")
        
        # Stats Section
        avgOut = np.mean(result)
        avgKnown = np.mean(xo.real)
        stdDeviation = np.std(xo.real)
        
        corr = signal.correlate(xo.real,result)
        elementer = corr.argmax()
        should = int((len(xo.real) + len(result))/2 - 1)

        error = np.zeros(min(len(xo.real), len(result)))
        error_mag = np.zeros(min(len(xo.real), len(result)))
        jj = 0
        # calculate absolute error relative to the standard deviation
        for ii in np.arange(should-elementer, min(len(xo.real) + should-elementer, len(result))):
            error[jj] = (xo.real[jj] - result[ii])
            error_mag[jj] = np.abs(xo.real[jj] - result[ii]) / stdDeviation
            jj+=1 
        
        if enablePlt:       
            import matplotlib.pyplot as plt    
            plt.figure()
            plt.subplot(3,1,1)
            plt.title('time signal real and imag')
            plt.plot(T, x.real, 'r')
            plt.plot(T, x.imag, 'g')
            plt.subplot(3,1,2)
            plt.title('time output test(r) actual(g)')
            plt.plot(To,xo.real, 'r')
            plt.plot(To,result, 'g')
            plt.subplot(3,1,3)
            plt.title('error')
            plt.plot(To,error_mag, 'b')           
            plt.show()
            
        self.assertEqual((0.01>avgOut-avgKnown),True)        
        self.assertEqual((5>np.max(np.abs(error_mag))),True)
        
        for tstamp in tstamps:
            self.assertEqual((tstamp[1].twsec>=0), True, "Whole Seconds should always be positive")
            self.assertEqual(((tstamp[1].tfsec>=0) and (tstamp[1].tfsec<1)), True, "Fractional Seconds should always be between zero and one")

        self.assertFalse(FAIL_TEST, "Test passed, failing assert for extra debug")
        
        
    def realToComplexShort2Short(self, scale=True, enablePlt=False):

        FAIL_TEST=0
        #DEBUG_LEVEL = 5
        #LOG_LEVEL = CF.LogLevels.DEBUG
        LOG_LEVEL = CF.LogLevels.INFO
        

        self.comp = sb.launch('../DataConverter.spd.xml')
        #self.comp = sb.launch('../DataConverter.spd.xml',execparams={'DEBUG_LEVEL':DEBUG_LEVEL})
        #self.comp = sb.launch('../DataConverter.spd.xml',properties={'DEBUG_LEVEL':DEBUG_LEVEL})
        self.comp.log_level(LOG_LEVEL)
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        self.comp.start()
        
        #set properties we care about
        self.comp.transformProperties.fftSize = 2048
        self.comp.transformProperties.lowCutoff = .001
        self.comp.transformProperties.highCutoff = .499
        self.comp.transformProperties.transitionWidth = .001
        self.comp.transformProperties.tune_fs_over_4 = False
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        self.comp.outputType=2
        
        #get a bunch of random data between 1 and -1 and create a BPSK Signal
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./32768)
        sint = np.sin(50000*t) * np.sin(1000*t)
        sintS = sint * 32767
        sintS = sintS.astype(np.short)
        
        streamID = "someSRI"
        src=sb.StreamSource(format="short",streamID= streamID)
        snk=sb.StreamSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataShort_in', usesPortName='shortOut')
        self.comp.connect(providesComponent=snk,providesPortName='shortIn',usesPortName='dataShort_out')
        
        sb.start()

        src.xdelta = 1.0
        src.complex = 0
        src.write(sintS.tolist(),interleaved=False)
        src.close()
        time.sleep(5) 

        self.assertEqual(streamID in snk.streamIDs(),True, "Expected Stream Not Present")
        streamdata = snk.read(timeout=1,streamID=streamID)
        result = streamdata.data

        sb.stop()
        self.comp.log_level(CF.LogLevels.INFO)
        self.comp.releaseObject()
        self.assertNotEqual(len(result), 0, "Did not receive pushed data!")
        
        nans = [1 for x in sintS if x != x]
        print 'len sintS, len nans:', len(sintS), len(nans)
        nans = [1 for x in result if x != x]
        print 'len result, len nans:', len(result), len(nans)

        res = np.array([np.complex(result[i]) for i in range(0,len(result))])
        
        h=signal.hilbert(sint) * 32767
        
        # Stats Section
        avgOut = np.mean(res)
        avgKnown = np.mean(h)
        stdDeviation = np.std(h)
        
        print 'avg res, avg h, stddev h:', avgOut, avgKnown, stdDeviation
        
        self.assertNotEqual(np.std(res), 0, 'Invalid output from DataConverter (likely due to NaN values in fftw processing chain)')

        corr = signal.correlate(h,res)
        elementer = corr.argmax()
        should = int((len(h) + len(res))/2 - 1)
        
        print 'len h, len res, should, elementer, should-elementer, corr:',len(h), len(res), should, elementer, should-elementer, corr
        
        self.assertNotEqual(elementer, 0, 'No correlation found between expected and actual result!')

        error_real = np.zeros(min(len(h), len(res)))
        error_imag = np.zeros(min(len(h), len(res)))
        error_mag = np.zeros(min(len(h), len(res)))
        error = np.array([np.complex(error_real[ii], error_imag[ii]) for ii in np.arange(len(error_imag))])
        
        self.assertTrue(elementer >= len(h), 'Bad correlation between expected and actual result!')
        
        jj = 0
        # calculate absolute error relative to the standard deviation
        print 'ii, jj, error_mag, h.real, res.real, h.imag, res.imag, stddev'
        for ii in np.arange(should-elementer, len(h) + should-elementer):
            error[jj] = np.complex((h.real[jj] - res.real[ii]), (h.imag[jj] - res.imag[ii]))
            error_mag[jj] = np.abs(np.complex((h.real[jj] - res.real[ii]), (h.imag[jj] - res.imag[ii]))) / stdDeviation
            if jj%1024 == 0: print ii, jj, error_mag[jj], h.real[jj], res.real[ii], h.imag[jj], res.imag[ii], stdDeviation
            jj+=1
        
        
        
        if enablePlt: 
            ter = np.arange(should-elementer, len(h) + should-elementer)
            import matplotlib.pyplot as plt
            plt.figure(1)
            plt.subplot(411)
            plt.title('Original sin(t)')
            plt.plot(sintS)        
            plt.subplot(412)
            plt.title('Real Arm from Component sin(t)')
            plt.plot(h[0:len(ter)-1].real, 'r')
            plt.plot(res[ter].real, 'g')
            plt.subplot(413)
            plt.title('Imag Arm from Component sin(t)')
            plt.plot(h[0:len(ter)-1].imag, 'r')
            plt.plot(res[ter].imag, 'g')        
            plt.subplot(414)
            plt.title('error_mag')
            plt.plot(error_mag)            
            plt.show()
                
        print 'avg res - avg h =', avgOut-avgKnown
        print 'max(abs(error_mag)):', np.max(np.abs(error_mag))
                
        self.assertEqual((0.01>avgOut-avgKnown),True)
        self.assertEqual((0.2>np.max(np.abs(error_mag))),True)

        self.assertFalse(FAIL_TEST, "Test passed, failing assert for extra debug")


    def complexToRealShort2Short(self, scale=True, enablePlt=False):

        FAIL_TEST=0
        #DEBUG_LEVEL = 5
        #LOG_LEVEL = CF.LogLevels.DEBUG
        LOG_LEVEL = CF.LogLevels.INFO
        

        self.comp = sb.launch('../DataConverter.spd.xml')#, debugger='gdb')
        #self.comp = sb.launch('../DataConverter.spd.xml',execparams={'DEBUG_LEVEL':DEBUG_LEVEL})
        #self.comp = sb.launch('../DataConverter.spd.xml',properties={'DEBUG_LEVEL':DEBUG_LEVEL})
        self.comp.log_level(LOG_LEVEL)
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about
        self.comp.outputType=1 
        
        Fs = 1024
        secs = 16
        num = 50
        Fcs = stats.norm.rvs(loc=0, scale=Fs/4,size=num)
        Phases = np.array([rand.uniform(-np.pi, np.pi) for ii in np.arange(num)]) 
#        Fcs = [1]
#        Phases = [0] 
        
        T = np.linspace(0,secs,Fs*secs)
        To = np.linspace(0,secs,Fs*2*secs)
        F = np.linspace(-Fs/2,Fs/2-1,Fs*secs)
        Fo = np.linspace(-Fs,Fs-1,Fs*2*secs)
        
        x = np.zeros(len(T), dtype=np.complex)
        
        for ii in np.arange(len(Fcs)):
            x = x+np.exp(2j*np.pi*Fcs[ii]*T + 1j*Phases[ii])
        
        
        outer = np.zeros(len(x)*2)
        for ii, val in enumerate(x):
            outer[2*ii] = val.real
            outer[2*ii+1] = val.imag
            
        maxOuter = np.max(np.abs(outer))
        outer = (outer / maxOuter) * 32767
        outer = outer.astype(np.short)
        
        x = x / maxOuter
        
        X = np.fft.fft(x)
        X_FFT = np.fft.fftshift(X)
        #for ii in np.arange(len(X)/2,len(X)):
        #    X[ii]= X[ii]#.conjugate()
        X = np.fft.fftshift(X)
        Xo = np.zeros(len(X)*2,dtype=np.complex)
        #Xo.put(np.arange(1,len(X)), X)
        #Xo.put(np.arange(len(X),len(X)*2), X[::-1].conjugate())
        Xo.put(np.arange(len(X)), X)
        Xo.put(np.arange(len(X)+1,len(X)*2), X[::-1].conjugate())
        Xo[0] = np.complex(Xo[0].real,0)
        #Xo[0] = np.complex(0,15)
        xo = np.fft.ifft(Xo)
        
        xo = xo * 32767
       
        streamID = "someSRI"
        src=sb.StreamSource(format="short",streamID= streamID)
        snk=sb.StreamSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataShort_in', usesPortName='shortOut')
        self.comp.connect(providesComponent=snk,providesPortName='shortIn',usesPortName='dataShort_out')
        
        sb.start()

        src.xdelta = 1.0/Fs
        src.complex = 1
        src.write(outer.tolist(),interleaved=True)
        src.close()
        time.sleep(5) 

        
        self.assertEqual(streamID in snk.streamIDs(),True, "Expected Stream Not Present")
        streamdata = snk.read(timeout=1,streamID=streamID)
        result = streamdata.data
        
                
        sb.stop()
        self.comp.log_level(CF.LogLevels.INFO)
        self.comp.releaseObject()
        self.assertNotEqual(len(result), 0, "Did not receive pushed data!")
        
        # Stats Section
        avgOut = np.mean(result)
        avgKnown = np.mean(xo.real)
        stdDeviation = np.std(xo.real)
        
        corr = signal.correlate(xo.real,result)
        elementer = corr.argmax()
        should = int((len(xo.real) + len(result))/2 - 1)

        error = np.zeros(min(len(xo.real), len(result)))
        error_mag = np.zeros(min(len(xo.real), len(result)))
        jj = 0
        # calculate absolute error relative to the standard deviation
        for ii in np.arange(should-elementer, len(xo.real) + should-elementer):
            error[jj] = (xo.real[jj] - result[ii])
            error_mag[jj] = np.abs(xo.real[jj] - result[ii]) / stdDeviation
            jj+=1 
        
        if enablePlt:       
            import matplotlib.pyplot as plt    
            plt.figure()
            plt.subplot(3,1,1)
            plt.title('time signal real and imag')
            plt.plot(T, x.real, 'r')
            plt.plot(T, x.imag, 'g')
            plt.subplot(3,1,2)
            plt.title('time output test(r) actual(g)')
            plt.plot(To,xo.real, 'r')
            plt.plot(To,result, 'g')
            plt.subplot(3,1,3)
            plt.title('error')
            plt.plot(To,error_mag, 'b')           
            plt.show()
                
        self.assertEqual((0.01>avgOut-avgKnown),True)        
        self.assertEqual((5>np.max(np.abs(error_mag))),True) #ToDo needs to change

        self.assertFalse(FAIL_TEST, "Test passed, failing assert for extra debug")



    def loopDataTypes(self,write=False,justPrint=False):
        if os.uname()[4].find('64') >-1:
            stdFile = 'standards_64.pkl'
        else:
            stdFile = 'standards_32.pkl'
        
        if (write):
            standardsFile = fopen(stdFile,'wb')
        else:
            standardsFile = fopen(stdFile,'rb')
        
        for compInType in self.types:
            src = bulkio_data_helpers.FileSource(eval(self.portTypes[compInType]))
            src_port = src.getPort()
            compInPort = self.inputPorts[compInType]
            src_port.connectPort(compInPort,"inputData")
            print "\n\n" + str(compInType)
            
            for compOutType in self.types:
                sink = bulkio_data_helpers     
                sink_port = sink.getPort()
                compOutPort = self.outputPorts[compOutType]
                compOutPort.connectPort(sink_port,'outputData')
                
                print "-->" + str(compOutType)
                
                src.run(self.inputFiles[compInType], self.sri,200000)
                time.sleep(0.2)
                
                if (write):
                    pickle.dump(sink.data, standardsFile)
                else:
                    stdLine = pickle.load(standardsFile)
                print "\t" + str(sink.data)
                
                if (justPrint == False):
                    count = 0
                    for stdVal in stdLine:
                        print count
                        print "data:\t\t" + str(sink.data[count])
                        print "standard:\t" + str(stdVal)
                        if compOutType == 'Float':
                            self.assertAlmostEqual(sink.data[count],stdVal,5)
                        else:
                            self.assertEqual(sink.data[count],stdVal)
                        count+=1
                    print "\tPASS"
        standardsFile.close()

    
    def testScaledChar2octet(self):      
        print(sys._getframe().f_code.co_name)
        self.char2octet(scale=True)
    
    def testChar2short(self):
        print(sys._getframe().f_code.co_name)
        self.char2short(scale=False)           
    
    def testScaledChar2short(self):
        print(sys._getframe().f_code.co_name)
        self.char2short(scale=True)
    
    def testScaledChar2ushort(self):
        print(sys._getframe().f_code.co_name)
        self.char2ushort(scale=True)

    def testChar2float(self):
        print(sys._getframe().f_code.co_name)
        self.char2float(scale=False)
    
    def testScaledChar2float(self):
        print(sys._getframe().f_code.co_name)
        self.char2float(scale=True)

    def testChar2double(self):
        print(sys._getframe().f_code.co_name)
        self.char2double(scale=False)
        
    def testScaledChar2double(self):
        print(sys._getframe().f_code.co_name)
        self.char2double(scale=True)
    
    def testChar2char(self):
        print(sys._getframe().f_code.co_name)
        self.char2char(scale=True)        
    
    def testScaledChar2char(self):
        print(sys._getframe().f_code.co_name)
        self.char2char(scale=True)
        
    def testScaledOctet2char(self):
        print(sys._getframe().f_code.co_name)
        self.octet2char(scale=True)
        
    def testScaledOctet2short(self):
        print(sys._getframe().f_code.co_name)
        self.octet2short(scale=True)
        
    def testOctet2ushort(self):
        print(sys._getframe().f_code.co_name)
        self.octet2ushort(scale=False)

    def testScaledOctet2ushort(self):
        print(sys._getframe().f_code.co_name)
        self.octet2ushort(scale=True)

    def testOctet2float(self):
        print(sys._getframe().f_code.co_name)
        self.octet2float(scale=False)

    def testScaledOctet2float(self):
        print(sys._getframe().f_code.co_name)
        self.octet2float(scale=True)
        
    def testOctet2double(self):
        print(sys._getframe().f_code.co_name)
        self.octet2double(scale=False)

    def testScaledOctet2double(self):
        print(sys._getframe().f_code.co_name)
        self.octet2double(scale=True)
        
    def testScaledShort2char(self):
        print(sys._getframe().f_code.co_name)
        self.short2char(scale=True)
        
    def testScaledShort2octet(self):
        print(sys._getframe().f_code.co_name)
        self.short2octet(scale=True)
        
    def testScaledShort2ushort(self):
        print(sys._getframe().f_code.co_name)
        self.short2ushort(scale=True)

    def testShort2float(self):
        print(sys._getframe().f_code.co_name)
        self.short2float(scale=False)
        
    def testScaledShort2float(self):
        print(sys._getframe().f_code.co_name)
        self.short2float(scale=True)

    def testShort2double(self):
        print(sys._getframe().f_code.co_name)
        self.short2double(scale=False)
        
    def testScaledShort2double(self):
        print(sys._getframe().f_code.co_name)
        self.short2double(scale=True)
        
    def testScaledUshort2char(self):
        print(sys._getframe().f_code.co_name)
        self.ushort2char(scale=True)
        
    def testScaledUshort2octet(self):
        print(sys._getframe().f_code.co_name)
        self.ushort2octet(scale=True)
        
    def testScaledUshort2short(self):
        print(sys._getframe().f_code.co_name)
        self.ushort2short(scale=True)

    def testUshort2float(self):
        print(sys._getframe().f_code.co_name)
        self.ushort2float(scale=False) 
        
    def testScaledUshort2float(self):
        print(sys._getframe().f_code.co_name)
        self.ushort2float(scale=True)
        
    def testScaledFloat2char(self):
        print(sys._getframe().f_code.co_name)
        self.float2char(scale=True)
        
    def testScaledFloat2octet(self):
        print(sys._getframe().f_code.co_name)
        self.float2octet(scale=True)
        
    def testScaledFloat2short(self):
        print(sys._getframe().f_code.co_name)
        self.float2short(scale=True)
        
    def testScaledFloat2ushort(self):
        print(sys._getframe().f_code.co_name)
        self.float2ushort(scale=True)

    def testFloat2float(self):
        print(sys._getframe().f_code.co_name)
        self.float2float(scale=False)

    def testScaledFloat2float(self):
        print(sys._getframe().f_code.co_name)
        self.float2float(scale=True)

    def testFloat2double(self):
        print(sys._getframe().f_code.co_name)
        self.float2double(scale=False)
        
    def testScaledFloat2double(self):
        print(sys._getframe().f_code.co_name)
        self.float2double(scale=True)
        
    def testScaledDouble2char(self):
        print(sys._getframe().f_code.co_name)
        self.double2char(scale=True)
        
    def testScaledDouble2octet(self):
        print(sys._getframe().f_code.co_name)
        self.double2octet(scale=True)
        
    def testScaledDouble2short(self):
        print(sys._getframe().f_code.co_name)
        self.double2short(scale=True)
        
    def testScaledDouble2ushort(self):
        print(sys._getframe().f_code.co_name)
        self.double2ushort(scale=True)
        
    def testDouble2float(self):
        print(sys._getframe().f_code.co_name)
        self.double2float(scale=False)

    def testScaledDouble2float(self):
        print(sys._getframe().f_code.co_name)
        self.double2float(scale=True)
    
    def testDouble2double(self):
        print(sys._getframe().f_code.co_name)
        self.double2double(scale=False)

    def testScaledDouble2double(self):
        print(sys._getframe().f_code.co_name)
        self.double2double(scale=True) 
        
    def testScaledRealToComplex(self):
        print(sys._getframe().f_code.co_name)
        self.realToComplex(scale=True)
        
    def testScaledRealToComplexShort2Short(self):
        print(sys._getframe().f_code.co_name)
        self.realToComplexShort2Short(scale=True)
        
    def testScaledComplexToReal(self):
        print(sys._getframe().f_code.co_name)
        self.complexToReal(scale=True)
        
    def testScaledComplexToRealShort2Short(self):
        print(sys._getframe().f_code.co_name)
        self.complexToRealShort2Short(scale=True)

    def testModeChangeR2CUpdatesSRI(self):
        print(sys._getframe().f_code.co_name)
        comp = sb.launch('../DataConverter.spd.xml')
        # TODO Update to stream source and sink 
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(comp,'dataDouble_in')
        comp.connect(snk,'ushortIn')
        
        comp.transformProperties.fftSize = 512
        
        sb.start()
        
        streamID = "someSRI"
        data = range(1024)
        src.push(data,complexData=False, sampleRate=1.0, EOS=False, streamID=streamID)

        # Wait for new SRI to take hold
        count = 0
        while (count < 100):
            if (snk.sri().streamID == streamID):
                break;
            count+=1
            time.sleep(0.05)

        sri1 = snk.sri()
        self.assertTrue(sri1.streamID == streamID)
        self.assertTrue(sri1.mode == 0) # 0=Scalar/Real
        #print 'len out 1', len(snk.getData())

        comp.outputType = 2 # 2=Complex
        
        src.push(data,complexData=False, sampleRate=1.0, EOS=False, streamID=streamID)
        #src.push(data,complexData=False, sampleRate=1.0, EOS=True, streamID=streamID)
        
        # Wait for new SRI to take hold
        count = 0
        while (count < 100):
            if (snk.sri().mode == 1):
                break;
            count+=1
            time.sleep(0.05)
        
        sri2 = snk.sri()
        #print 'len out 2', len(snk.getData())
        
        self.assertTrue(sri2.streamID == streamID)
        self.assertTrue(sri2.mode == 1) # 1=Complex
        
        sb.stop()
        comp.releaseObject()
    
    def testModeChangeC2RUpdatesSRI(self):
        print(sys._getframe().f_code.co_name)
        LOG_LEVEL = CF.LogLevels.TRACE
        comp = sb.launch('../DataConverter.spd.xml')
        #comp.log_level(LOG_LEVEL)
        streamID = "someSRI"
        src=sb.StreamSource(format="double",streamID= streamID)
        snk=sb.StreamSink()
        
        src.connect(comp,'dataDouble_in')
        comp.connect(snk,'ushortIn')
        
        comp.transformProperties.fftSize = 512
        
        sb.start()
        data = range(1024)*2

        src.xdelta = 1.0 
        src.complex = 1
        src.write(data,interleaved=True)
        # Wait for new SRI to take hold
        count = 0
        while (count < 100):
            if streamID in snk.streamIDs():
                break;
            count+=1
            time.sleep(0.05)

        self.assertEqual(streamID in snk.streamIDs(),True, "Expected Stream Not Present")
        streamdata = snk.read(timeout=1,streamID=streamID)
        self.assertTrue(streamdata.streamID == streamID)
        #Defaults to pass through and input data is Complex
        self.assertTrue(streamdata.sri.mode == 1)
        #print 'len out 1', len(streamdata.data))

        #Change output type to be real
        comp.outputType = 1 # 1=Real
        time.sleep(.1)
        src.write(data,interleaved=True)
        streamdata2 = snk.read(timeout=2,streamID=streamID)
        #print 'len out 2', len(streamdata.data)
        self.assertTrue(streamdata2.streamID == streamID)
        self.assertTrue(streamdata2.sri.mode == 0)
        
        sb.stop()
        comp.releaseObject()

    def testTwoInstances(self): # TODO - convert to stream source/sink
        print(sys._getframe().f_code.co_name)
        LOG_LEVEL = CF.LogLevels.TRACE
        
        # first instance
        comp = sb.launch('../DataConverter.spd.xml')
        comp.log_level(LOG_LEVEL)
        src=sb.DataSource()
        snk=sb.DataSink()
        src.connect(comp,'dataShort_in')
        comp.connect(snk,'ushortIn')
        comp.transformProperties.fftSize = 512
        comp.outputType = 1 # 1=Real
        
        # second instance
        comp2 = sb.launch('../DataConverter.spd.xml')
        comp2.log_level(LOG_LEVEL)
        src2=sb.DataSource()
        snk2=sb.DataSink()
        src2.connect(comp2,'dataShort_in')
        comp2.connect(snk2,'ushortIn')
        comp2.transformProperties.fftSize = 512
        comp2.outputType = 2 # 2=Complex
        
        sb.start()
        
        streamID = "someSRI"
        data = range(1024)*4
        src.push(data,complexData=True, sampleRate=1.0, EOS=True, streamID=streamID+'1')
        src2.push(data,complexData=False, sampleRate=1.0, EOS=True, streamID=streamID+'2')

        # Wait for new SRI to take hold
        count = 0
        while (count < 100):
            if (snk.sri().streamID == streamID+'1'):
                break;
            count+=1
            time.sleep(0.05)
        count = 0
        while (count < 100):
            if (snk2.sri().streamID == streamID+'2'):
                break;
            count+=1
            time.sleep(0.05)
        
        sb.stop()
        comp.releaseObject()
        comp2.releaseObject()

    def testSRIandStreamBehavior(self):
        print(sys._getframe().f_code.co_name)
        comp = sb.launch('../DataConverter.spd.xml')

        streamID = "myStream"
        streamID2 = "DifferentStream"
        src=sb.StreamSource(format="double",streamID= streamID)
        src2=sb.StreamSource(format="double",streamID= streamID2)
        snk=sb.StreamSink()
        
        src.connect(comp,'dataDouble')
        src2.connect(comp,'dataDouble')
        comp.connect(snk,'ushortIn')
        
        sb.start()

        data = range(1024)

        src.setKeyword('kw1',1000)
        src.xdelta = 1 
        src.complex = 1
        src.write(data,interleaved=True)
        #src.push(data,complexData=True, sampleRate=1.0, EOS=False, streamID=streamID,SRIKeywords=kws)    
        time.sleep(1)
        
        #Verify Output Data SRI
        self.assertEqual(streamID in snk.streamIDs(),True, "Expected Stream Not Present")
        streamdata = snk.read(timeout=1,streamID=streamID)
        self.assertEqual(streamdata.streamID,streamID, "Output StreamID does not match")
        self.assertEqual(streamdata.sri.mode, 1, "Output mode does not match, should be complex, mode=1")
        self.assertEqual(streamdata.eos,False, "Stream sent an EOS")
        self.assertAlmostEqual(streamdata.sri.xdelta, 1.0, "X-delta Does Not Match")
        self.assertEqual(streamdata.sri.keywords[0].id, 'kw1', "Keyword id does not match expected")
        self.assertEqual(any.from_any(streamdata.sri.keywords[0].value), 1000, "Keyword value does not match expected")


        #Push more Data with same stream ID no EOS 
        src.write(data,interleaved=True)
        
        # Push more data from a different stream ID no EOS
        src2.setKeyword('kw1',1000)
        src2.xdelta = 1 
        src2.complex = 1
        src2.write(data,interleaved=True)

        time.sleep(1)
         
        #  Verify all good output data.
        self.assertEqual(streamID in snk.streamIDs(),True, "Expected Stream Not Present")
        streamdata = snk.read(timeout=1,streamID=streamID)
        self.assertEqual(streamdata.streamID,streamID, "Output StreamID does not match")
        self.assertEqual(streamdata.sri.mode, 1, "Output mode does not match, should be complex, mode=1")
        self.assertEqual(streamdata.eos,False, "Stream sent an EOS")
        self.assertAlmostEqual(streamdata.sri.xdelta, 1.0, "X-delta Does Not Match")
        self.assertEqual(streamdata.sri.keywords[0].id, 'kw1', "Keyword id does not match expected")
        self.assertEqual(any.from_any(streamdata.sri.keywords[0].value), 1000, "Keyword value does not match expected")
 
        self.assertEqual(streamID2 in snk.streamIDs(),True, "Expected Stream Not Present")
        streamdata = snk.read(timeout=1,streamID=streamID2)
        self.assertEqual(streamdata.streamID,streamID2, "Output StreamID does not match")
        self.assertEqual(streamdata.sri.mode, 1, "Output mode does not match, should be complex, mode=1")
        self.assertEqual(streamdata.eos,False, "Stream sent an EOS")
        self.assertAlmostEqual(streamdata.sri.xdelta, 1.0, "X-delta Does Not Match")
        self.assertEqual(streamdata.sri.keywords[0].id, 'kw1', "Keyword id does not match expected")
        self.assertEqual(any.from_any(streamdata.sri.keywords[0].value), 1000, "Keyword value does not match expected")        
         
        # Stop Component
        comp.stop()        
         
        # Update SRI and push more data        
        src.eraseKeyword('kw1')
        src.setKeyword('kw2',2000)
        src.xdelta = 1/2.0
        src.write(data,interleaved=True)

 
        # Start component
        comp.start() 
        time.sleep(1)
         
        # Push more data same two streams
        src.write(data,interleaved=True)
        src.close()
        src2.write(data,interleaved=True)
        
        time.sleep(1)        
        # Verify Output Data and SRI, EOS   
        self.assertEqual(streamID in snk.streamIDs(),True, "Expected Stream Not Present")
        streamdata = snk.read(timeout=1,streamID=streamID)
        self.assertEqual(streamdata.streamID,streamID, "Output StreamID does not match")
        self.assertEqual(streamdata.sri.mode, 1, "Output mode does not match, should be complex, mode=1")
        self.assertEqual(streamdata.eos,True, "Stream sent an EOS")
        self.assertAlmostEqual(streamdata.sri.xdelta, 0.5, "X-delta Does Not Match")
        self.assertEqual(streamdata.sri.keywords[0].id, 'kw2', "Keyword id does not match expected")
        self.assertEqual(any.from_any(streamdata.sri.keywords[0].value), 2000, "Keyword value does not match expected")
 
        self.assertEqual(streamID2 in snk.streamIDs(),True, "Expected Stream Not Present")
        streamdata = snk.read(timeout=1,streamID=streamID2)
        self.assertEqual(streamdata.streamID,streamID2, "Output StreamID does not match")
        self.assertEqual(streamdata.sri.mode, 1, "Output mode does not match, should be complex, mode=1")
        self.assertEqual(streamdata.eos,False, "Stream sent an EOS")
        self.assertAlmostEqual(streamdata.sri.xdelta, 1.0, "X-delta Does Not Match")
        self.assertEqual(streamdata.sri.keywords[0].id, 'kw1', "Keyword id does not match expected")
        self.assertEqual(any.from_any(streamdata.sri.keywords[0].value), 1000, "Keyword value does not match expected") 
         
        # Stop Component and push data without SRI updates. 
        comp.stop()
        
        src2.write(data,interleaved=True)
 
        comp.start()
        time.sleep(1)
 
        self.assertEqual(streamID2 in snk.streamIDs(),True, "Expected Stream Not Present")
        streamdata = snk.read(timeout=1,streamID=streamID2)
        self.assertEqual(streamdata.streamID,streamID2, "Output StreamID does not match")
        self.assertEqual(streamdata.sri.mode, 1, "Output mode does not match, should be complex, mode=1")
        self.assertEqual(streamdata.eos,False, "Stream sent an EOS")
        self.assertAlmostEqual(streamdata.sri.xdelta, 1.0, "X-delta Does Not Match")
        self.assertEqual(streamdata.sri.keywords[0].id, 'kw1', "Keyword id does not match expected")
        self.assertEqual(any.from_any(streamdata.sri.keywords[0].value), 1000, "Keyword value does not match expected") 
         
        sb.stop()
        comp.releaseObject()
         

    def testScaBasicBehavior(self):
        print(sys._getframe().f_code.co_name)
        #######################################################################
        # Launch the component with the default execparams
        execparams = self.getPropertySet(kinds=("execparam",), modes=("readwrite", "writeonly"), includeNil=False)
        execparams = dict([(x.id, any.from_any(x.value)) for x in execparams])
        self.launch(execparams)
        
        #######################################################################
        # Verify the basic state of the component
        self.assertNotEqual(self.comp, None)
        self.assertEqual(self.comp.ref._non_existent(), False)
        self.assertEqual(self.comp.ref._is_a("IDL:CF/Resource:1.0"), True)
        
        #######################################################################
        # Simulate regular component startup
        # Verify that initialize nor configure throw errors
        self.comp.initialize()
        configureProps = self.getPropertySet(kinds=("configure",), modes=("readwrite", "writeonly"), includeNil=False)
        self.comp.configure(configureProps)
        
        #######################################################################
        # Validate that query returns all expected parameters
        # Query of '[]' should return the following set of properties
        expectedProps = []
        expectedProps.extend(self.getPropertySet(kinds=("configure", "execparam"), modes=("readwrite", "readonly"), includeNil=True))
        expectedProps.extend(self.getPropertySet(kinds=("allocate",), action="external", includeNil=True))
        props = self.comp.query([])
        props = dict((x.id, any.from_any(x.value)) for x in props)
        # Query may return more than expected, but not less
        for expectedProp in expectedProps:
            self.assertEquals(props.has_key(expectedProp.id), True)
        
        #######################################################################
        # Verify that all expected ports are available
        for port in self.scd.get_componentfeatures().get_ports().get_uses():
            port_obj = self.comp.getPort(str(port.get_usesname()))
            self.assertNotEqual(port_obj, None)
            self.assertEqual(port_obj._non_existent(), False)
            self.assertEqual(port_obj._is_a("IDL:CF/Port:1.0"),  True)
            
        for port in self.scd.get_componentfeatures().get_ports().get_provides():
            port_obj = self.comp.getPort(str(port.get_providesname()))
            self.assertNotEqual(port_obj, None)
            self.assertEqual(port_obj._non_existent(), False)
            self.assertEqual(port_obj._is_a(port.get_repid()),  True)
            
        #######################################################################
        # Make sure start and stop can be called without throwing exceptions
        self.comp.start()
        self.comp.stop()
        
        #######################################################################
        # Simulate regular component shutdown
        self.comp.releaseObject()
     

    
if __name__ == "__main__":
    ossie.utils.testing.main("../DataConverter.spd.xml") # By default tests all implementations
