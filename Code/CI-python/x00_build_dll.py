# -*- coding: utf-8 -*-

import os
import sys
import subprocess

dictProjectPath = {
    'databaseLib' : 'Code/databaseLib/IKDatabaseLib/Build',
    'serial' : 'Code/serial/serial/Build',
    'SerialMuxDemux' : 'Code/SerialMuxDemux/SerialMuxDemux/Build',
    'AlgApi' : 'Code/AlgApi_eclipse/Build',
    'xml' : 'Code/XML/xml/Build',
    'ikIOStream' : 'Code/ikIOStream/Build',
    'IrisManager' : 'Code/IrisAppLib/IrisManager/Build',
    'SocketComm' : 'Code/SocketComm/Build',
    'USBAPI' : 'Code/USBAPI/Build',
    ## '' : 'Code//Debug',
    }

#################### FUNCTION DEFINE: printCmd() ####################
def printCmd (cmdTuple):
    'convert command from tuple type to string type for usage firendly'
    buildCmd = ''
    for cmd in cmdTuple:
        buildCmd += cmd + ' '
    print (buildCmd)
    
#################### END OF FUNCTION: printCmd() ####################

#################### FUNCTION DEFINE: buildProject() ####################
def buildProject(projKey):
    'form building command and create a new process'
    ## 查找字典中是否有指定的工程，如果没有则不做编译
    if (projKey not in dictProjectPath):
        print('Sorry, can not find this project :', projKey);
        sys.exit(2)

    ## 形成编译命令行
    projCmd = dictProjectPath[projKey]

    ## 先clean原来的结果
    try:
    	x00BuildCmd = ('make', '-C', projCmd, 'clean')
	printCmd(x00BuildCmd)
	##pThread = subprocess.Popen((x00BuildCmd))
	##pThread.wait()
        subprocess.check_call((x00BuildCmd))
    except Exception as err:
        print(err)
        sys.exit(1)
        

    try:
        x00BuildCmd = ('make', '-C', projCmd, 'all')
        ##pThread = subprocess.Popen((x00BuildCmd))
        ##pThread.wait()
        retCode = subprocess.check_call(x00BuildCmd)
        ##print('return value is %d' %(retCode))
    except Exception as err:
        print(err)
        sys.exit(1)

    ##return True

#################### END OF FUNCTION: buildProject() ####################


#################### FUNCTION DEFINE: thisMain() ####################
def thisMain():
    'main function of this program'

    try:
        return buildProject(sys.argv[1])

    except Exception as err:
        print(err)
        sys.exit(2)
#################### END OF FUNCTION: thisMain() ####################
    
#################### Main Program ####################
if __name__ == '__main__':
    thisMain()
