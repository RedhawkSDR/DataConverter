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

import cmath
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

    def initializeDicts(self):
        self.types = ['Char', 'Octet', 'Short', 'Ushort', 'Float', 'Double']
        self.inputPorts = {}
        self.outputPorts = {}
        self.portTypes = {}
        self.inputFiles = {}
        for type in self.types:
            self.inputPorts[type] = self.comp_obj.getPort('data' + str(type))
            self.outputPorts[type] = self.comp.getPort('data' + str(type) + '_out')
            self.portTypes[type] = 'BULKIO__POA.data' + str(type)
            self.inputFiles[type] = str(type) + '.dat'
        self.sri = BULKIO.StreamSRI(1,0.0,0.001,1,200,0.0,0.001,1,1,"dataConvTest", 0 , [])
        self.sri.xdelta = 1.0/400
        self.sri.mode = 0
        self.length = 32

    def char2char(self,scale=True):
        print "-----------------------------char2char---------------------------"
                
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.shortPort = scale
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        minChar = -2**7-1
        sint = np.sin(50000*t)
        sinwaveArray = np.array([0]*len(sint),'int8')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'int8')
        
        sMin = -128
        sRange = 255
        dMin = -128
        dRange = 255 
        
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*minChar) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(dRange/sRange)+float(dMin)

        src=sb.DataSource(dataFormat='char')
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataChar', usesPortName='charOut')
        self.comp.connect(providesComponent=snk,providesPortName='charIn',usesPortName='dataChar_out')
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
                
        result = snk.getData()
        self.comp.releaseObject()
        for i in range(0,len(result)):
            self.assertEqual(np.int8(result[i]),cast_out[i])                   
        if len(result)!=0:
            print "PASS - Char to Char"


    def char2octet(self,scale=True):
        print "-----------------------------char2uchar---------------------------"
        
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.octetPort = scale

        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sinwaveArray = np.array([0]*len(sint),'int8')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'uint8')
        sMin = -128
        sRange = 255
        dMin = 0
        dRange = 255
        
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sMin) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(dRange/sRange)+float(dMin)

        src=sb.DataSource(dataFormat='char')
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataChar', usesPortName='charOut')
        self.comp.connect(providesComponent=snk,providesPortName='octetIn',usesPortName='dataOctet_out')      
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)

        result = snk.getData()
        sb.stop()
        self.comp.releaseObject()
        
        
                
        for i in range(0,len(result)):
                self.assertEqual(cast_out[i],ord(result[i]))        
        if len(result)!=0:
            print "PASS - Char to Octet"

    def char2short(self, scale=True):
        print "-----------------------------char2short---------------------------"
        
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.shortPort = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        minChar = -2**7-1
        sint = np.sin(50000*t)
        sinwaveArray = np.array([0]*len(sint),'int8')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'int16')
        
        sMin = -128
        sRange = 255
        dMin = -32768
        dRange = 65535 
        
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sMin) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(dRange/sRange)+float(dMin)

        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataChar', usesPortName='charOut')
        self.comp.connect(providesComponent=snk,providesPortName='shortIn',usesPortName='dataShort_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()

        sb.stop()
        self.comp.releaseObject()

        for i in range(0,len(result)):
            self.assertEqual(cast_out[i],result[i])
        if len(result)!=0:
            print "PASS - Char to Short"

    def char2ushort(self, scale=True):
        print "-----------------------------char2ushort---------------------------"
        
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.ushortPort = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        minChar = -2**7-1
        sint = np.sin(50000*t)
        sMin = -128
        sRange = 255
        dMin = 0
        dRange = 65536
        sinwaveArray = np.array([0]*len(sint),'int8')
        self.comp.scaleOutput.ushortPort = True
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'uint16')

        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sMin) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(dRange/sRange)+float(dMin)
                
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataChar', usesPortName='charOut')
        self.comp.connect(providesComponent=snk,providesPortName='ushortIn',usesPortName='dataUshort_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        
        for i in range(0,len(result)):
            self.assertEqual(cast_out[i],result[i])
        if len(result)!=0:
            print "PASS - Char to UShort"
        
        sb.stop()
        self.comp.releaseObject()
    
    def char2float(self, scale=True):
        print "-----------------------------char2float---------------------------"
        
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = -128
        sRange = 255
        dMin = -1
        dRange = 2
        sinwaveArray = np.array([0]*len(sint),'int8')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'float')
        #print str(float(Fraction(dRange,sRange)))
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sMin) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)
        
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataChar', usesPortName='charOut')
        self.comp.connect(providesComponent=snk,providesPortName='floatIn',usesPortName='dataFloat_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        
        for i in range(0,len(result)):
            self.assertAlmostEqual(cast_out[i],result[i],5)
        if len(result) !=0:
            print "PASS - Char to Float"

        sb.stop()
        self.comp.releaseObject()
    
    def char2double(self, scale=True):
        print "-----------------------------char2double---------------------------"
        
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        
        t = np.array([],dtype='double')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = -128
        sRange = 255
        dMin = -1
        dRange = 2
        sinwaveArray = np.array([0]*len(sint),'int8')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'float')
        #print str(float(Fraction(dRange,sRange)))
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sMin) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)
        
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataChar', usesPortName='charOut')
        self.comp.connect(providesComponent=snk,providesPortName='doubleIn',usesPortName='dataDouble_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        
        for i in range(0,len(result)):
            self.assertAlmostEqual(cast_out[i],result[i],5)
        if len(result) !=0:
            print "PASS - Char to Double"

        sb.stop()
        self.comp.releaseObject()
    
    def octet2char(self,scale=True):
        print "-----------------------------octet2char---------------------------"
                
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.charPort = scale
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        minChar = -2**7-1
        sint = np.sin(50000*t)
        sinwaveArray = np.array([0]*len(sint),'uint8')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'int8')
        
        sMin = 0
        sRange = 255
        dMin = -128
        dRange = 255 
        
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sRange) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(dRange/sRange)+float(dMin)

        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataOctet', usesPortName='octetOut')
        self.comp.connect(providesComponent=snk,providesPortName='charIn',usesPortName='dataChar_out')
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
                
        result = snk.getData()
        self.comp.releaseObject()
        for i in range(0,len(result)):
            #self.assertEqual(np.int8(ord(result[i])),cast_out[i])
            self.assertEqual(np.int8(result[i]),cast_out[i])
        if len(result) !=0:
            print "PASS - Octet to Char"


    def octet2octet(self,scale=True):
        print "-----------------------------octet2uchar---------------------------"
        
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        #self.comp.scaleOutput.octetPort = scale

        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sinwaveArray = np.array([0]*len(sint),'uint8')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'uint8')
        sMin = 0
        sRange = 255
        dMin = 0
        dRange = 255
        
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sRange) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(dRange/sRange)+float(dMin)

        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataOctet', usesPortName='octetOut')
        self.comp.connect(providesComponent=snk,providesPortName='octetIn',usesPortName='dataOctet_out')      
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)

        result = snk.getData()
        sb.stop()
        self.comp.releaseObject()
        print len(result)
        for i in range(0,len(cast_out)):
                self.assertEqual(cast_out[i],ord(result[i]))        
        if len(result) !=0:
            print "PASS - Char to Octet"

    def octet2short(self, scale=True):
        print "-----------------------------octet2short---------------------------"
        
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.shortPort = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sinwaveArray = np.array([0]*len(sint),'uint8')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'int16')
        
        sMin = 0
        sRange = 255
        dMin = -32768
        dRange = 65535 
        
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sRange) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(dRange/sRange)+float(dMin)

        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataOctet', usesPortName='octetOut')
        self.comp.connect(providesComponent=snk,providesPortName='shortIn',usesPortName='dataShort_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()

        sb.stop()
        self.comp.releaseObject()

        for i in range(0,len(result)):
            #print str(cast_out[i]) + " " + str(result[i])
            self.assertEqual(cast_out[i],result[i])        
        if len(result) !=0:
            print "PASS - octet to Short"

    def octet2ushort(self, scale=True):
        print "-----------------------------octet2ushort---------------------------"
        
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.ushortPort = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = 0
        sRange = 255
        dMin = 0
        dRange = 65536
        sinwaveArray = np.array([0]*len(sint),'uint8')
        self.comp.scaleOutput.ushortPort = scale
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'uint16')

        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sRange) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(dRange/sRange)+float(dMin)
                
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataOctet', usesPortName='octetOut')
        self.comp.connect(providesComponent=snk,providesPortName='ushortIn',usesPortName='dataUshort_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        for i in range(0,len(result)):
            self.assertEqual(cast_out[i],result[i])
        if len(result) !=0:
            print "PASS - octet to UShort"

        sb.stop()
        self.comp.releaseObject()
    
    def octet2float(self, scale=True):
        print "-----------------------------octect2float---------------------------"
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = 0
        sRange = 255
        dMin = -1
        dRange = 2
        sinwaveArray = np.array([0]*len(sint),'uint8')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'float')
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sRange) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)
        
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataOctet', usesPortName='octetOut')
        self.comp.connect(providesComponent=snk,providesPortName='floatIn',usesPortName='dataFloat_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        for i in range(0,len(result)):
            self.assertAlmostEqual(cast_out[i],result[i],5)
        print "PASS - octet to Float"

        sb.stop()
        self.comp.releaseObject()
    
    def octet2double(self, scale=True):
        print "-----------------------------octect2double---------------------------"
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = 0
        sRange = 255
        dMin = -1
        dRange = 2
        sinwaveArray = np.array([0]*len(sint),'uint8')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'float')
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sRange) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)
        
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataOctet', usesPortName='octetOut')
        self.comp.connect(providesComponent=snk,providesPortName='doubleIn',usesPortName='dataDouble_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        for i in range(0,len(result)):
            self.assertAlmostEqual(cast_out[i],result[i],5)
        print "PASS - octet to double"

        sb.stop()
        self.comp.releaseObject()
    
    def short2char(self,scale=True):
        print "-----------------------------short2char---------------------------"
                
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.charPort = scale
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        sint = np.sin(50000*t)
        sinwaveArray = np.array([0]*len(sint),'int16')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'int8')
        
        sMin = -32768
        sRange = 65535
        dMin = -128
        dRange = 255 
        
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sMin) 
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)

        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataShort', usesPortName='shortOut')
        self.comp.connect(providesComponent=snk,providesPortName='charIn',usesPortName='dataChar_out')
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
                
        result = snk.getData()

        self.comp.releaseObject()
        count = 0;
        # since there is a fraction involved and rounding can be different, we will check for == =+1 or =-1        
        for i in range(0,len(result)):
            #if (cast_out[i]+1 == np.int8(ord(result[i])) or cast_out[i]-1 == np.int8(ord(result[i])) or cast_out[i] == np.int8(ord(result[i]))):
            if (cast_out[i]+1 == np.int8(result[i]) or cast_out[i]-1 == np.int8(result[i]) or cast_out[i] == np.int8(result[i])):
                count = count + 1
        if count == len(result) and count !=0:        
                print "PASS - Short to char"
        
    def short2octet(self,scale=True):
        print "-----------------------------short2octet---------------------------"
        
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.octetPort = scale

        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sinwaveArray = np.array([0]*len(sint),'int8')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'uint8')
        sMin = -32768
        sRange = 65535
        dMin = 0
        dRange = 255
        
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sMin) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)

        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataShort', usesPortName='shortOut')
        self.comp.connect(providesComponent=snk,providesPortName='octetIn',usesPortName='dataOctet_out')      
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)

        result = snk.getData()
        sb.stop()
        self.comp.releaseObject()
        count = 0
        # since there is a fraction involved and rounding can be different, we will check for == =+1 or =-1                       
        for i in range(0,len(result)):
                if (cast_out[i]+1 == ord(result[i]) or cast_out[i]-1 == ord(result[i]) or cast_out[i] == ord(result[i])):
                        count = count + 1
        if count == len(result) and count !=0:        
                print "PASS - Short to octet"                

    def short2ushort(self, scale=True):
        print "-----------------------------short2ushort---------------------------"
                
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.ushortPort = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = -32768
        sRange = 65535
        dMin = 0
        dRange = 65536
        sinwaveArray = np.array([0]*len(sint),'int16')
        self.comp.scaleOutput.ushortPort = scale
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'uint16')

        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sMin) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(dRange/sRange)+float(dMin)
                
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataShort', usesPortName='shortOut')
        self.comp.connect(providesComponent=snk,providesPortName='ushortIn',usesPortName='dataUshort_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        for i in range(0,len(result)):
            self.assertEqual(cast_out[i],result[i])
        if len(result) !=0:
            print "PASS - Short to UShort"

        sb.stop()
        self.comp.releaseObject()

    def short2float(self, scale=True):
        print "-----------------------------short2float---------------------------"
        
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = -32768
        sRange = 65535
        dMin = -1
        dRange = 2
        sinwaveArray = np.array([0]*len(sint),'int16')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'float')
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sRange) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)
        
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataShort', usesPortName='shortOut')
        self.comp.connect(providesComponent=snk,providesPortName='floatIn',usesPortName='dataFloat_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        
        for i in range(0,len(result)):
            self.assertAlmostEqual(cast_out[i],result[i],5)
        if len(result) !=0:
            print "PASS - Short to Float"

        sb.stop()
        self.comp.releaseObject()        
    
    def short2double(self, scale=True):
        print "-----------------------------short2double---------------------------"
        
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = -32768
        sRange = 65535
        dMin = -1
        dRange = 2
        sinwaveArray = np.array([0]*len(sint),'int16')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'float')
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sRange) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)
        
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataShort', usesPortName='shortOut')
        self.comp.connect(providesComponent=snk,providesPortName='doubleIn',usesPortName='dataDouble_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        
        for i in range(0,len(result)):
            self.assertAlmostEqual(cast_out[i],result[i],5)
        if len(result) !=0:
            print "PASS - Short to Double"

        sb.stop()
        self.comp.releaseObject()        
    
    def ushort2char(self,scale=True):
        print "-----------------------------ushort2char---------------------------"
                
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.charPort = scale
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        sint = np.sin(50000*t)
        sinwaveArray = np.array([0]*len(sint),'uint16')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'int8')
        
        sMin = 0
        sRange = 65535
        dMin = -128
        dRange = 255 
        
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sRange) 
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)

        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataUshort', usesPortName='ushortOut')
        self.comp.connect(providesComponent=snk,providesPortName='charIn',usesPortName='dataChar_out')
        sb.start()
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
                
        result = snk.getData()
        self.comp.releaseObject()
        count = 0;
        # since there is a fraction involved and rounding can be different, we will check for == =+1 or =-1        
        for i in range(0,len(result)):
            #if (cast_out[i]+1 == np.int8(ord(result[i])) or cast_out[i]-1 == np.int8(ord(result[i])) or cast_out[i] == np.int8(ord(result[i]))):
            if (cast_out[i]+1 == np.int8(result[i]) or cast_out[i]-1 == np.int8(result[i]) or cast_out[i] == np.int8(result[i])):
                count = count + 1
        if count == len(result) and count !=0:        
                print "PASS - Ushort to char"
        
    def ushort2octet(self,scale=True):
        print "-----------------------------ushort2octet---------------------------"
        
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.octetPort = scale

        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sinwaveArray = np.array([0]*len(sint),'uint16')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'uint8')
        sMin = 0
        sRange = 65535
        dMin = 0
        dRange = 255
        
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sRange) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)

        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataUshort', usesPortName='ushortOut')
        self.comp.connect(providesComponent=snk,providesPortName='octetIn',usesPortName='dataOctet_out')      
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)

        result = snk.getData()
        sb.stop()
        self.comp.releaseObject()
        count = 0
        # since there is a fraction involved and rounding can be different, we will check for == =+1 or =-1                       
        for i in range(0,len(result)):
                if (cast_out[i]+1 == ord(result[i]) or cast_out[i]-1 == ord(result[i]) or cast_out[i] == ord(result[i])):
                        count = count + 1
        if count == len(result) and count !=0:        
                print "PASS - UShort to octet"

    def ushort2short(self, scale=True):
        print "-----------------------------ushort2short---------------------------"
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.shortPort = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = 0
        sRange = 65535
        dMin = -32768
        dRange = 65536
        sinwaveArray = np.array([0]*len(sint),'uint16')
        self.comp.scaleOutput.ushortPort = scale
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'int16')

        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sRange) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(dRange/sRange)+float(dMin)
                
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataUshort', usesPortName='ushortOut')
        self.comp.connect(providesComponent=snk,providesPortName='shortIn',usesPortName='dataShort_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        for i in range(0,len(result)):
            self.assertEqual(cast_out[i],result[i])
        if len(result) !=0:
            print "PASS - UShort to Short"

        sb.stop()
        self.comp.releaseObject()

    def ushort2float(self, scale=True):
        print "-----------------------------uShort2float---------------------------"
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = 0
        sRange = 65535
        dMin = -1
        dRange = 2
        sinwaveArray = np.array([0]*len(sint),'uint16')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'float')
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sRange) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)
        
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataUshort', usesPortName='ushortOut')
        self.comp.connect(providesComponent=snk,providesPortName='floatIn',usesPortName='dataFloat_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        
        for i in range(0,len(result)):
            self.assertAlmostEqual(cast_out[i],result[i],5)
        if  len(result) !=0:
            print "PASS - UShort to Float"

        sb.stop()
        self.comp.releaseObject()
    
    def ushort2double(self, scale=True):
        print "-----------------------------uShort2double---------------------------"
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = 0
        sRange = 65535
        dMin = -1
        dRange = 2
        sinwaveArray = np.array([0]*len(sint),'uint16')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'float')
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sRange) 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)
        
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataUshort', usesPortName='ushortOut')
        self.comp.connect(providesComponent=snk,providesPortName='doubleIn',usesPortName='dataDouble_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        print sinwaveArray.tolist()
        print result
        
        for i in range(0,len(result)):
            self.assertAlmostEqual(cast_out[i],result[i],5)
        if  len(result) !=0:
            print "PASS - UShort to double"

        sb.stop()
        self.comp.releaseObject()
        
    def float2char(self,scale=True):
        print "-----------------------------float2char---------------------------"
                
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.charPort = scale
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        sint = np.sin(50000*t)
        sinwaveArray = np.array([0]*len(sint),'float')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'int8')
        
        sMin = -1
        sRange = 2
        dMin = -128
        dRange = 255 
        
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sMin) 
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)

        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataFloat', usesPortName='floatOut')
        self.comp.connect(providesComponent=snk,providesPortName='charIn',usesPortName='dataChar_out')
        sb.start()
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
                
        result = snk.getData()
        self.comp.releaseObject()
        count = 0;
        # since there is a fraction involved and rounding can be different, we will check for == =+1 or =-1        
        for i in range(0,len(result)):
            #if (cast_out[i]+1 == np.int8(ord(result[i])) or cast_out[i]-1 == np.int8(ord(result[i])) or cast_out[i] == np.int8(ord(result[i]))):
            if (cast_out[i]+1 == np.int8(result[i]) or cast_out[i]-1 == np.int8(result[i]) or cast_out[i] == np.int8(result[i])):
                count = count + 1
        if len(result) == len(result)  and count !=0:        
            print "PASS - Float to char"

    def float2octet(self,scale=True):
        print "-----------------------------float2octet---------------------------"
        
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.octetPort = scale
        self.comp.normalize_floating_point.Input = True
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = -1
        sRange = 2
        dMin = 0
        dRange = 255
        sinwaveArray = np.array([0]*len(sint),'float')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'uint16')
        for i in range(0,len(sint)):
                sinwaveArray[i]=float(sint[i]*sMin)
                if (sinwaveArray[i] == -0):
                    sinwaveArray[i] = 0 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = round(float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin))
        
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataFloat', usesPortName='floatOut')
        self.comp.connect(providesComponent=snk,providesPortName='octetIn',usesPortName='dataOctet_out')      
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)

        result = snk.getData()
        sb.stop()
        self.comp.releaseObject()
        count = 0

        self.assertEqual(len(cast_in), len(result), "Number of pushed elements does not match number of received elements")

        # since there is a fraction involved and rounding can be different, we will check for == =+1 or =-1                       
        for i in range(0,len(result)):
            self.assertTrue(abs(cast_out[i] - ord(result[i])) <= 1)
        print "PASS - Float to Octet"

    def float2short(self,scale=True):
        print "-----------------------------float2short---------------------------"
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = -1
        sRange = 2
        dMin = -32768
        dRange = 65535
        sinwaveArray = np.array([0]*len(sint),'float')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'int16')
        for i in range(0,len(sint)):
                sinwaveArray[i]=float(sint[i]*sMin)
                if (sinwaveArray[i] == -0):
                    sinwaveArray[i] = 0                
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)
                
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataFloat', usesPortName='floatOut')
        self.comp.connect(providesComponent=snk,providesPortName='shortIn',usesPortName='dataShort_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        count = 0
        #because of rounding errors look +/- 1 of the value         
        for i in range(0,len(result)):
            if (cast_out[i]+1 == result[i] or cast_out[i]-1 == result[i] or cast_out[i] == result[i]):
                count = count + 1
        if count == len(result)  and count !=0:        
                print "PASS - float to short"
        
        sb.stop()
        self.comp.releaseObject()        
    
    def float2ushort(self,scale=True):
        print "-----------------------------float2ushort---------------------------"
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = -1
        sRange = 2
        dMin = 0
        dRange = 65535
        sinwaveArray = np.array([0]*len(sint),'float')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'uint16')
        for i in range(0,len(sint)):
                sinwaveArray[i]=float(sint[i]*sMin)
                if (sinwaveArray[i] == -0):
                    sinwaveArray[i] = 0 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = round(float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin))
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataFloat', usesPortName='floatOut')
        self.comp.connect(providesComponent=snk,providesPortName='ushortIn',usesPortName='dataUshort_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        count = 0
        #because of rounding errors look +/- 1 of the value         
        for i in range(0,len(result)):
            if (cast_out[i]+1 == result[i] or cast_out[i]-1 == result[i] or cast_out[i] == result[i]):
                count = count + 1
        if count == len(result) and count !=0:        
                print "PASS - float to ushort"
    
    def float2double(self,scale=True):
        print "-----------------------------float2double---------------------------"
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = -1
        sRange = 2
        dMin = 0
        dRange = 65535
        sinwaveArray = np.array([0]*len(sint),'float')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'double')
        for i in range(0,len(sint)):
                sinwaveArray[i]=float(sint[i]*sMin)
                if (sinwaveArray[i] == -0):
                    sinwaveArray[i] = 0 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)
        src=sb.DataSource(bytesPerPush=10)
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataFloat', usesPortName='floatOut')
        self.comp.connect(providesComponent=snk,providesPortName='doubleIn',usesPortName='dataDouble_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        (result,tstamps) = snk.getData(tstamps=True)
        count = 0
        
        #because of rounding errors look +/- 1 of the value
        self.assertEqual(len(cast_in), len(result), "Number of pushed elements does not match number of received elements")
        data = sinwaveArray.tolist()
        for i in range(0,len(result)):
            self.assertTrue(abs(data[i] - result[i]) < 0.00001)
            
        # Default for Data Source is Start time 0 and sample rate 1, therefore each sample offset should be equal to the whole number of seconds.
        for tstamp in tstamps:
            self.assertEqual(tstamp[0],tstamp[1].twsec)
            self.assertEqual(0,tstamp[1].tfsec)
            
        print "PASS - float to double"

   
    
    def double2char(self,scale=True):
        print "-----------------------------double2char---------------------------"
                
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.charPort = scale
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        sint = np.sin(50000*t)
        sinwaveArray = np.array([0]*len(sint),'float')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'int8')
        
        sMin = -1
        sRange = 2
        dMin = -128
        dRange = 255 
        
        for i in range(0,len(sint)):
                sinwaveArray[i]=int(sint[i]*sMin) 
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)

        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataDouble', usesPortName='doubleOut')
        self.comp.connect(providesComponent=snk,providesPortName='charIn',usesPortName='dataChar_out')
        sb.start()
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
                
        result = snk.getData()
        self.comp.releaseObject()
        count = 0;
        # since there is a fraction involved and rounding can be different, we will check for == =+1 or =-1        
        for i in range(0,len(result)):
            #if (cast_out[i]+1 == np.int8(ord(result[i])) or cast_out[i]-1 == np.int8(ord(result[i])) or cast_out[i] == np.int8(ord(result[i]))):
            if (cast_out[i]+1 == np.int8(result[i]) or cast_out[i]-1 == np.int8(result[i]) or cast_out[i] == np.int8(result[i])):
                count = count + 1
        if len(result) == len(result)  and count !=0:        
            print "PASS - Double to char"
    
    def double2octet(self,scale=True):
        print "-----------------------------double2octet---------------------------"
        
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.scaleOutput.charPort = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = -1
        sRange = 2
        dMin = 0
        dRange = 255
        sinwaveArray = np.array([0]*len(sint),'float')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'uint16')
        for i in range(0,len(sint)):
                sinwaveArray[i]=float(sint[i]*sMin)
                if (sinwaveArray[i] == -0):
                    sinwaveArray[i] = 0 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = round(float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin))
        
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataDouble', usesPortName='doubleOut')
        self.comp.connect(providesComponent=snk,providesPortName='octetIn',usesPortName='dataOctet_out')      
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)

        result = snk.getData()
        sb.stop()
        self.comp.releaseObject()
        count = 0

        self.assertEqual(len(cast_in), len(result), "Number of pushed elements does not match number of received elements")

        # since there is a fraction involved and rounding can be different, we will check for == =+1 or =-1                       
        for i in range(0,len(result)):
            self.assertTrue(abs(cast_out[i] - ord(result[i])) <= 1)
        print "PASS - Double to Octet"

    def double2short(self,scale=True):
        print "-----------------------------double2short---------------------------"
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = -1
        sRange = 2
        dMin = -32768
        dRange = 65535
        sinwaveArray = np.array([0]*len(sint),'float')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'int16')
        for i in range(0,len(sint)):
                sinwaveArray[i]=float(sint[i]*sMin)
                if (sinwaveArray[i] == -0):
                    sinwaveArray[i] = 0                
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)
                
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataDouble', usesPortName='doubleOut')
        self.comp.connect(providesComponent=snk,providesPortName='shortIn',usesPortName='dataShort_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result= snk.getData()
        
        count = 0
        #because of rounding errors look +/- 1 of the value         
        for i in range(0,len(result)):
            if (cast_out[i]+1 == result[i] or cast_out[i]-1 == result[i] or cast_out[i] == result[i]):
                count = count + 1
        if count == len(result)  and count !=0:        
                print "PASS - double to short"
        
        sb.stop()
        self.comp.releaseObject()        
    
    def double2ushort(self,scale=True):
        print "-----------------------------double2ushort---------------------------"
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = -1
        sRange = 2
        dMin = 0
        dRange = 65535
        sinwaveArray = np.array([0]*len(sint),'float')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'uint16')
        for i in range(0,len(sint)):
                sinwaveArray[i]=float(sint[i]*sMin)
                if (sinwaveArray[i] == -0):
                    sinwaveArray[i] = 0 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = round(float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin))
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataDouble', usesPortName='doubleOut')
        self.comp.connect(providesComponent=snk,providesPortName='ushortIn',usesPortName='dataUshort_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        count = 0
        #because of rounding errors look +/- 1 of the value         
        for i in range(0,len(result)):
            if (cast_out[i]+1 == result[i] or cast_out[i]-1 == result[i] or cast_out[i] == result[i]):
                count = count + 1
        if count == len(result) and count !=0:        
                print "PASS - double to ushort"
    
    def double2float(self,scale=True):
        print "-----------------------------double2float---------------------------"
        #each test is its own run
        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about        
        self.comp.normalize_floating_point.Input = scale
        self.comp.normalize_floating_point.Output = scale
        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./(self.length))
        
        sint = np.sin(50000*t)
        sMin = -1
        sRange = 2
        dMin = 0
        dRange = 65535
        sinwaveArray = np.array([0]*len(sint),'float')
        cast_in = np.array([0]*len(sint),'float')
        cast_out= np.array([0]*len(sint),'double')
        for i in range(0,len(sint)):
                sinwaveArray[i]=float(sint[i]*sMin)
                if (sinwaveArray[i] == -0):
                    sinwaveArray[i] = 0 
                cast_in[i] = float(sinwaveArray[i])
                cast_out[i] = float(sinwaveArray[i]-sMin)*float(Fraction(dRange,sRange))+float(dMin)
        src=sb.DataSource(bytesPerPush=10)
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataDouble', usesPortName='doubleOut')
        self.comp.connect(providesComponent=snk,providesPortName='floatIn',usesPortName='dataFloat_out')
        
        sb.start()
        
        src.push(data=sinwaveArray.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        (result,tstamps) = snk.getData(tstamps=True)
        count = 0
        
        self.assertEqual(len(cast_in), len(result), "Number of pushed elements does not match number of received elements")
        data = sinwaveArray.tolist()
        for i in range(0,len(result)):
            self.assertTrue(abs(data[i] - result[i]) < 0.00001)
            
        for tstamp in tstamps:
            self.assertEqual(tstamp[0],tstamp[1].twsec)
            self.assertEqual(0,tstamp[1].tfsec)

        print "PASS - double to float"

    def realToComplex(self, enablePlt=False):
        print "-----------------------------RealToComplex---------------------------"

        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        self.comp.start()
        write = False
        justPrint = False
        sb.setDEBUG(False)
        
        self.comp.transformProperties.fftSize = 2048
        self.comp.transformProperties.lowCutoff = .001
        self.comp.transformProperties.highCutoff = .499
        self.comp.transformProperties.transitionWidth = .001
        self.comp.transformProperties.tune_fs_over_4 = False
        self.comp.normalize_floating_point.Output = True
        self.comp.normalize_floating_point.Input = True

        self.comp.outputType = 2 
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./32768)
        #get a bunch of random data between 1 and -1 and create a BPSK Signal
        

        sint = np.sin(50000*t) * np.sin(1000*t)
        src=sb.DataSource()#bytesPerPush=512 + 32)
        snk=sb.DataSink()

        src.connect(self.comp, "dataFloat")
        self.comp.connect(snk, "floatIn")
        
        sb.start()
        src.push(data=sint.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        sb.stop()
        self.comp.releaseObject()
        self.assertNotEqual(len(result), 0, "Did not receive pushed data!")

        res = np.array([np.complex(result[ii], result[ii+1]) for ii in np.arange(0,len(result)-1,2)])
        
        h=signal.hilbert(sint)
        hReal = h.real
        hImag = h.imag
        
        # Stats Section
        avgOut = np.mean(res)
        avgKnown = np.mean(h)
        stdDeviation = np.std(h)

        corr = signal.correlate(h,res)
        elementer = corr.argmax()
        should = int((len(h) + len(res))/2 - 1)

        error_real = np.zeros(min(len(h), len(res)))
        error_imag = np.zeros(min(len(h), len(res)))
        error_mag = np.zeros(min(len(h), len(res)))
        error = np.array([np.complex(error_real[ii], error_imag[ii]) for ii in np.arange(len(error_imag))])
        jj = 0
        # calculate absolute error relative to the standard deviation
        for ii in np.arange(should-elementer, len(h) + should-elementer):
            error[jj] = np.complex((h.real[jj] - res.real[ii]), (h.imag[jj] - res.imag[ii]))
            error_mag[jj] = np.abs(np.complex((h.real[jj] - res.real[ii]), (h.imag[jj] - res.imag[ii]))) / stdDeviation
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
                
        self.assertEqual((0.01>avgOut-avgKnown),True)
        self.assertEqual((0.1>np.max(np.abs(error_mag))),True)

        print "PASS - RealToComplex"



    def complexToReal(self, enablePlt=False):
        print "-----------------------------ComplexToReal---------------------------"

        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        #set properties we care about
        
        self.comp.transformProperties.fftSize = 2048
        self.comp.transformProperties.lowCutoff = .001
        self.comp.transformProperties.highCutoff = .499
        self.comp.transformProperties.transitionWidth = .001
        self.comp.transformProperties.tune_fs_over_4 = False
        self.comp.normalize_floating_point.Output = True
        self.comp.normalize_floating_point.Input = True
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

        src=sb.DataSource()#bytesPerPush=512+32)
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataFloat', usesPortName='floatOut')
        self.comp.connect(providesComponent=snk,providesPortName='floatIn',usesPortName='dataFloat_out')
        
        sb.start()
        src.push(data=outer.tolist(),sampleRate=Fs,EOS=True,complexData=True)
        time.sleep(1)
        result = snk.getData()
        sb.stop()
        self.comp.releaseObject()
        
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

        print "PASS - ComplexToReal"
        
        
        
    def realToComplexShort2Short(self, enablePlt=False):
        print "-----------------------------RealToComplexShort2Short---------------------------"

        self.comp = sb.launch('../DataConverter.spd.xml')
        self.comp_obj=self.comp.ref
        self.initializeDicts()
        self.comp.start()
        write = False
        justPrint = False
        sb.setDEBUG(False)
        
        self.comp.transformProperties.fftSize = 2048
        self.comp.transformProperties.lowCutoff = .001
        self.comp.transformProperties.highCutoff = .499
        self.comp.transformProperties.transitionWidth = .001
        self.comp.transformProperties.tune_fs_over_4 = False
        self.comp.normalize_floating_point.Output = True
        self.comp.normalize_floating_point.Input = True


        self.comp.outputType=2        
        t = np.array([],dtype='float')
        t = np.arange(0,2*np.pi/400.,2*np.pi/400./32768)
        #get a bunch of random data between 1 and -1 and create a BPSK Signal
        

        sint = np.sin(50000*t) * np.sin(1000*t)
        sintS = sint * 32767
        sintS = sintS.astype(np.short)
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataShort', usesPortName='shortOut')
        self.comp.connect(providesComponent=snk,providesPortName='shortIn',usesPortName='dataShort_out')
        
        sb.start()
        src.push(data=sintS.tolist(),EOS=True,complexData=False)
        time.sleep(1)
        result = snk.getData()
        sb.stop()
        self.comp.releaseObject()

        res = np.array([np.complex(result[ii], result[ii+1]) for ii in np.arange(0,len(result)-1,2)])
        
        h=signal.hilbert(sint) * 32767
        
        # Stats Section
        avgOut = np.mean(res)
        avgKnown = np.mean(h)
        stdDeviation = np.std(h)

        corr = signal.correlate(h,res)
        elementer = corr.argmax()
        should = int((len(h) + len(res))/2 - 1)

        error_real = np.zeros(min(len(h), len(res)))
        error_imag = np.zeros(min(len(h), len(res)))
        error_mag = np.zeros(min(len(h), len(res)))
        error = np.array([np.complex(error_real[ii], error_imag[ii]) for ii in np.arange(len(error_imag))])
        jj = 0
        # calculate absolute error relative to the standard deviation
        for ii in np.arange(should-elementer, len(h) + should-elementer):
            error[jj] = np.complex((h.real[jj] - res.real[ii]), (h.imag[jj] - res.imag[ii]))
            error_mag[jj] = np.abs(np.complex((h.real[jj] - res.real[ii]), (h.imag[jj] - res.imag[ii]))) / stdDeviation
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
                
        self.assertEqual((0.01>avgOut-avgKnown),True)
        self.assertEqual((0.2>np.max(np.abs(error_mag))),True)

        print "PASS - RealToComplexShort2Short"


    def complexToRealShort2Short(self, enablePlt=False):
        print "-----------------------------ComplexToRealShort2Short---------------------------"

        self.comp = sb.launch('../DataConverter.spd.xml')#, debugger='gdb')
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

        src=sb.DataSource(bytesPerPush=512+32)
        snk=sb.DataSink()
        
        src.connect(providesComponent=self.comp,providesPortName = 'dataShort', usesPortName='shortOut')
        self.comp.connect(providesComponent=snk,providesPortName='shortIn',usesPortName='dataShort_out')
        
        sb.start()
        src.push(data=outer.tolist(),sampleRate=Fs,EOS=True,complexData=True)
        time.sleep(2)
        result = snk.getData()
        sb.stop()
        self.comp.releaseObject()
        
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

        print "PASS - ComplexToRealShort2Short"



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

    def testScaledFunctionality(self):
        print "\n----------------------------- testScaledFunctionality-------------------------"
        execparams = self.getPropertySet(kinds=("execparam",), modes=("readwrite", "writeonly"), includeNil=False)
        execparams = dict([(x.id, any.from_any(x.value)) for x in execparams])


        self.char2octet()        
        self.char2short()        
        self.char2ushort()
        self.char2float()
        self.char2double()
        self.char2char()
        self.octet2char()
        self.octet2short()
        self.octet2ushort()
        self.octet2float()
        self.octet2double()
        self.short2char()
        self.short2octet()
        self.short2ushort()
        self.short2float()
        self.short2double()
        self.ushort2char()
        self.ushort2octet()
        self.ushort2short()
        self.ushort2float()
        self.float2char()
        self.float2octet()        
        self.float2short()
        self.float2ushort()
        self.float2double()
        self.double2char()
        self.double2octet()        
        self.double2short()
        self.double2ushort()
        self.double2float()
# The following tests are commented out due to a bug in the sandbox regarding the DataSource object, and EOS flags  
        self.realToComplex()
        self.realToComplexShort2Short()
        self.complexToReal()
        self.complexToRealShort2Short()       

    def testModeChangeR2CUpdatesSRI(self):
        print "\n----------------------------- testModeChange(R->C)UpdatesSRI-------------------------"
        comp = sb.launch('../DataConverter.spd.xml')
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(comp,'dataDouble')
        comp.connect(snk,'ushortIn')
        
        sb.start()
        
        streamID = "someSRI"
        data = range(1024)
        src.push(data,complexData=False, sampleRate=1.0, EOS=False, streamID=streamID)

        # Wait for new SRI to take hold
        count = 0
        while (count < 100):
            if (snk.sri().streamID == streamID):
                break;
            time.sleep(0.05)

        self.assertTrue(snk.sri().streamID == streamID)
        self.assertTrue(snk.sri().mode == 0) # 0=Real

        comp.outputType = 2 # 2=Complex
        self.assertTrue(snk.sri().streamID == streamID)
        self.assertTrue(snk.sri().mode == 1) # 1=Complex
    
    def testModeChangeC2RUpdatesSRI(self):
        print "\n----------------------------- testModeChange(C->R)UpdatesSRI-------------------------"
        comp = sb.launch('../DataConverter.spd.xml')
        src=sb.DataSource()
        snk=sb.DataSink()
        
        src.connect(comp,'dataDouble')
        comp.connect(snk,'ushortIn')
        
        sb.start()
        
        streamID = "someSRI"
        data = range(1024)
        src.push(data,complexData=True, sampleRate=1.0, EOS=False, streamID=streamID)

        # Wait for new SRI to take hold
        count = 0
        while (count < 100):
            if (snk.sri().streamID == streamID):
                break;
            time.sleep(0.05)

        self.assertTrue(snk.sri().streamID == streamID)
        self.assertTrue(snk.sri().mode == 1)

        comp.outputType = 1 # 1=Real
        self.assertTrue(snk.sri().streamID == streamID)
        self.assertTrue(snk.sri().mode == 0)

    def testScaBasicBehavior(self):
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
        
    # TODO Add additional tests here
    #
    # See:
    #   ossie.utils.bulkio.bulkio_helpers,
    #   ossie.utils.bluefile.bluefile_helpers
    # for modules that will assist with testing components with BULKIO ports
    
if __name__ == "__main__":
    ossie.utils.testing.main("../DataConverter.spd.xml") # By default tests all implementations
