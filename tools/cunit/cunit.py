import os, sys,string
import re
'''
def deal_file(src):
    # file exist or not
    if not os.path.exists(src):
        print 'Error: file - %s doesn\'t exist.'% src
        return False
    if os.path.islink(src):
        print 'Error: file - %s is a link.'
        return False
    filetype = (os.path.splitext(src))[1]
    if not filetype in ['.c','.h','.cpp','.hh','.cc']:
        return False
    try:
        if not os.access(src, os.W_OK):
            os.chmod(src, 0664)
    except:
        print 'Error: you can not chang %s\'s mode.'% src
 
    inputf = open(src, 'r')
    outputfilename = (os.path.splitext(src))[0] + '_no_comment'+filetype
    outputf = open(outputfilename, 'w')    
 
    try:
        #-----find /*.....*/
        rFlag=0
        line=inputf.readline()
        while(line):
            fm=string.find(line,'/*')
            if fm!=-1:  # find a /*
                if fm>1:# not empty line
                    outputf.write(line[:fm]+'\n')
                rFlag=1
                # find */
                fm=string.find(line,'*/')
                if fm!=-1:
                    rFlag=0
                else:
                    line=inputf.readline()
                    while line:
                        fm=string.find(line,'*/')
                        if fm!=-1:
                            rFlag=0
                            break
                        line=inputf.readline()
                    if not line:
                        print 'Match /*...*/ error'
            else: # deal with //
                fm=string.find(line,'//')
                if fm==-1:
                    if len(line)>1: # not empty line
                        outputf.write(line)
                elif fm!=-1 and (not rFlag):
                    if fm>1: # not empty line
                        outputf.write(line[:fm]+'\n')
            #read nextline
            line=inputf.readline()
    except:
        print 'Error: unexcept error.'
        inputf.close()
        outputf.close()
    return True
'''

class Header:
    def __init__(self, rule):
        self.content = "#include <CUnit/CUnit.h>\n"
        self.content += "#include <CUnit/TestDB.h>\n\n"
        self.array = "extern CU_TestInfo %s[];\n\n" %(rule)
        self.initFun=""
        self.cleanFun=""
        self.testFun=""
    
    def addTest(self, fun):
        self.array += fun+";\n"
        self.testFun += fun[fun.find(" ")+1:fun.find("(")]
        self.testFun += ";"

    def getTestFunList(self):
        return self.testFun

    def addSuiteInit(self, fun):
        self.initFun = fun

    def addSuiteClean(self, fun):
        self.cleanFun = fun
        
    def getContent(self):
        self.array += "%s;\n%s;\n" %(self.initFun, self.cleanFun)
        return self.content + self.array

   
class Source:
    def __init__(self):
        self.content = "#include <CUnit/CUnit.h>\n\n"
        self.array = "CU_SuiteInfo suites[] =\n{\n"
        self.initFun = ""
        self.cleanFun = ""
        self.arg = ""
        
    def addRule(self, rule):
        self.content += "#include \"%s\"\n\n" %(rule)
        self.testArray = ""
        self.array += "    {\"%s\"," %(rule.split('.')[0])
        self.arg = ""

    def addTestArray(self, rule, testFunList):
        self.testArray = "CU_TestInfo %s[] = {\n" %(rule.split('.')[0])
        num=testFunList.count(";")
        for i in range(num):
            self.testArray += "    {\"%s:\", %s},\n" %(testFunList.split(';')[i], testFunList.split(';')[i])
        self.testArray += "    CU_TEST_INFO_NULL\n};\n\n"

    def addInitFun(self, fun):
        self.initFun = fun[fun.find(" ")+1: fun.find("(")]

    def addCleanFun(self, fun):
        self.cleanFun = fun[fun.find(" ")+1: fun.find("(")]

    def addArg(self, arg):
        index = arg.find(" ")
        self.arg = arg[arg.find(" ", index+1)+1:arg.find("[")]
    
    def addEntry(self):
        self.array += "%s, %s, NULL, NULL, %s},\n" %(self.initFun, self.cleanFun, self.arg)
        
    def getContent(self):
        self.array += "    CU_SUITE_INFO_NULL\n};\n\n"
        return self.content + self.testArray + self.array
    
class Robot:
    def __init__(self):
        self.src = Source()
        self.hdrDict = dict()
        
    def writeSource(self, file):
        self.write(file, self.src.getContent())
    
    def writeHeader(self, rule, file):
        self.write(file, self.hdrDict[rule].getContent())

    def addRule(self, rule):
        self.src.addRule(rule)

    def addEntry(self):
        self.src.addEntry();
    
    def addHdr(self, rule):
        self.hdrDict[rule] = Header(rule)

    def parseSrc(self, rule, line):
        result = self.parseTest(line)
        if result:
            hdr = self.hdrDict[rule]
            hdr.addTest(result.group(0))

        result = self.parseSuiteInit(line)
        if result:
            hdr = self.hdrDict[rule]
            hdr.addSuiteInit(result.group(0))

        result = self.parseSuiteClean(line)
        if result:
            hdr = self.hdrDict[rule]
            hdr.addSuiteClean(result.group(0))

        return

    def parseHdr(self, rule, line):
        result = self.parseSuiteInit(line)
        if result:
            self.src.addInitFun(result.group(0))
            return

        result = self.parseSuiteClean(line)
        if result:
            self.src.addCleanFun(result.group(0))
            return

        result = self.parseArg(line)
        if result:
            self.src.addArg(result.group(0))
            return
        
        return

    def parseArg(self, line):
        arg = re.compile(r"extern CU_TestInfo\s+\w+\[\]")
        result = arg.match(line)
        return result

    def parseTest(self, line):
        testFun = re.compile(r"void\s+test_\w+\(void\)")
        result = testFun.match(line)
        return result

    def parseSuiteInit(self, line):
        suiteInitFun = re.compile(r"int\s+suite_\w*init\(void\)")
        result = suiteInitFun.match(line)
        return result

    def parseSuiteClean(self, line):
        suiteCleanFun = re.compile(r"int\s+suite_\w*clean\(void\)")
        result = suiteCleanFun.match(line)
        return result

    def write(self, file, content):
        fp = open(file,"w");
        fp.write(content)
        fp.flush()
        fp.close()

    def addTestArray(self, rule):
        self.src.addTestArray(rule, self.hdrDict[os.path.splitext(rule)[0]].getTestFunList())

def main():
    if len (sys.argv) < 2:
        print sys.argv[0] + " <rule> "
        return
    robot = Robot()
    for i in range(1, len(sys.argv)):
        path = os.path.dirname(sys.argv[i])
        filename = sys.argv[i][len(path)+1:]
        robot.addHdr(os.path.splitext(filename)[0])
        for line in open(sys.argv[i]):
            robot.parseSrc(os.path.splitext(filename)[0], line)
        robot.writeHeader(os.path.splitext(filename)[0], "tools/cunit/"+os.path.splitext(filename)[0]+".h")

    source = "tools/cunit/cunit.c"
    for i in range(1, len(sys.argv)):
        path = os.path.dirname(sys.argv[i])
        filename = sys.argv[i][len(path)+1:]
        rule = os.path.splitext(filename)[0]+".h"
        robot.addRule(rule)
        robot.addTestArray(rule)
        for line in open("tools/cunit/"+rule):  
            try:
                robot.parseHdr(rule, line.lstrip())
            except SystemExit,e:
                print "%s, in rule %d line %d!\n" %(e, rule, line)
                exit(-1)
        robot.addEntry()
    robot.writeSource(source)

if __name__ == "__main__":
    main()
