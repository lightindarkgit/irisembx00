# -*- coding: utf-8 -*-

import os
import sys
import subprocess

dictProjectPath = {
    'UI' : 'Code/ikembx00UI',
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
    ## 查找字典中是否有制定的工程，如果没有则不做编译
    if (projKey not in dictProjectPath):
        print('Sorry, can not find this project :', projKey);
        return False

    ## 形成编译命令行
    projCmd = dictProjectPath[projKey]

    ## 通过ikembx00.pro生成makefile
    proName = os.path.join(projCmd, 'ikembx00.pro')
    makefileName = os.path.join(projCmd, 'Makefile')
    
    x00BuildCmd = ('/usr/share/qt4/bin/qmake', '-makefile', \
                   proName, '-o' , makefileName)
    
    printCmd(x00BuildCmd)
    pThread = subprocess.Popen((x00BuildCmd))
    pThread.wait()

    ## 先clean原来的结果
    x00BuildCmd = ('make', '-C', projCmd, 'clean')
    printCmd(x00BuildCmd)
    pThread = subprocess.Popen((x00BuildCmd))
    pThread.wait()

    x00BuildCmd = ('make', '-C', projCmd)
    pThread = subprocess.Popen((x00BuildCmd))
    pThread.wait()
    
    
#################### END OF FUNCTION: buildProject() ####################


#################### FUNCTION DEFINE: thisMain() ####################
def thisMain():
    'main function of this program'

    try:
        return buildProject(sys.argv[1])

    except Exception as err:
        print(err)
        return False
#################### END OF FUNCTION: thisMain() ####################
    
#################### Main Program ####################
if __name__ == '__main__':
    thisMain()
