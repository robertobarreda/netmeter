#!/usr/bin/env python

from XmlIndent import XMLIndentGenerator
import re, sys,  time

def getSeqPattern(mgenVersion):
    """Builds the RegExp Object depending on the Mgen version"""
    if mgenVersion == 3:
        return re.compile (r'''
        Flow>(\d+)\s+
        Seq>(\d+)\s+
        Src>\s+(\d{1,3}.\d{1,3}.\d{1,3}.\d{1,3})/(\d+)\s+
        Dest>\s+(\d{1,3}.\d{1,3}.\d{1,3}.\d{1,3})/(\d+)\s+
        TxTime>(\d{2}:\d{2}:\d{2}.\d{6})\s+
        RxTime>(\d{2}:\d{2}:\d{2}.\d{6})\s+
        Size>(\d+)
        ''', re.VERBOSE)
    elif mgenVersion == 4:
        return re.compile (r'''
        (\d{2}:\d{2}:\d{2}.\d{6})\s+
        RECV\s+	
        flow>(\d+)\s+
        seq>(\d+)\s+
        src>(\d{1,3}.\d{1,3}.\d{1,3}.\d{1,3})/(\d+)\s+
        dst>(\d{1,3}.\d{1,3}.\d{1,3}.\d{1,3})/(\d+)\s+
        sent>(\d{2}:\d{2}:\d{2}.\d{6})\s+
        size>(\d+)\s+\w+
        ''', re.VERBOSE)

##### MGEN3 Support #####
def compareSeq3(a,  b):
    if int(a[0]) < int(b[0]) : return -1
    elif int(a[0]) > int(b[0]) : return 1
    else:
            if int(a[1]) <= int(b[1]) : return -1
            else : return 1

def writeMgen3(flowList, output):
    lastFlow = -1

    print 'writing MGEN3 in', output
    out = open( output,  'w' )

    gen = XMLIndentGenerator(out)
    gen.startDocument()
    gen.startElement('test')
    gen.startElement('metadata')
    gen.writeTextElement('type', 'packets')
    gen.writeTextElement('version', '3')
    gen.endElement()
    gen.startElement('result')
    gen.startElement('flows')

    for seq in flowList:
        if int(seq[0]) != lastFlow:
            if lastFlow != -1:
                gen.endElement()
            lastFlow = int(seq[0])
            gen.startElement('flow', id=str(lastFlow))
            gen.startElement('source')
            gen.writeTextElement('address', seq[2])
            gen.writeTextElement('port', seq[3])
            gen.endElement()
            gen.startElement('destination')
            gen.writeTextElement('address', seq[4])
            gen.writeTextElement('port', seq[5])
            gen.endElement()
        gen.startElement('seq',  id=str(int(seq[1])))
        gen.writeTextElement('tx_time', seq[6])
        gen.writeTextElement('rx_time', seq[7])
        gen.writeTextElement('size', str(int(seq[8])))
        gen.endElement()
    gen.endElement()
    gen.endElement()
    gen.endElement()
    gen.endElement()
    gen.endDocument()
    out.close()


##### MGEN4 Support #####
def compareSeq4(a,  b):
    if int(a[1]) < int(b[1]) : return -1
    elif int(a[1]) > int(b[1]) : return 1
    else:
            if int(a[2]) <= int(b[2]) : return -1
            else : return 1

def writeMgen4(flowList, output):
    lastFlow = -1

    print 'writing MGEN4 in', output
    out = open( output,  'w' )

    gen = XMLIndentGenerator(out)
    gen.startDocument()
    gen.startElement('test')
    gen.startElement('metadata')
    gen.writeTextElement('type', 'packets')
    gen.writeTextElement('version', '4')
    gen.endElement()
    gen.startElement('result')
    gen.startElement('flows')

    for seq in flowList:
        if int(seq[1]) != lastFlow:
            if lastFlow != -1:
                gen.endElement()
            lastFlow = int(seq[1])
            gen.startElement('flow', id=seq[1])
            gen.startElement('source')
            gen.writeTextElement('address', seq[3])
            gen.writeTextElement('port', seq[4])
            gen.endElement()
            gen.startElement('destination')
            gen.writeTextElement('address', seq[5])
            gen.writeTextElement('port', seq[6])
            gen.endElement()
        gen.startElement('seq',  id=seq[2])
        gen.writeTextElement('tx_time', seq[7])
        gen.writeTextElement('rx_time', seq[0])
        gen.writeTextElement('size', seq[8])
        gen.endElement()
    gen.endElement()
    gen.endElement()
    gen.endElement()
    gen.endElement()
    gen.endDocument()
    out.close()

def convert( typeMgen, inFile, outFile = "/tmp/output.xff" ):
    print "CONVERT [",str(typeMgen),",",inFile,",",outFile,"]"
    flowList = []
    seqPattern = getSeqPattern( typeMgen )
    f = open( inFile, 'r' )
    for linea in f:
	result = seqPattern.match( linea )
	if result: flowList.append( result.groups() ) 
    f.close()

    if typeMgen == 3:
        flowList.sort( compareSeq3 )
        writeMgen3( flowList, outFile )
    elif typeMgen == 4:
        flowList.sort( compareSeq4 )
        writeMgen4( flowList, outFile )

if __name__ == '__main__':
	if len( sys.argv ) == 4:
		start = time.time()	
		print 'START PARSING!', time.time() - start
		convert(int(sys.argv[1]), sys.argv[2], sys.argv[3])
		print 'END! WRITTEN IN',  time.time() - start

	else:
		print 'Usage: mgen.py {3|4} <source_file> <dest_file>'
