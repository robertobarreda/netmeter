"""Generate indented XML.

An extension of the XMLGenerator class from the SAX interface which provides
features for generating indented XML. The method 'newline' in the extension
class XMLIndentGenerator outputs a newline character and the appropriate
number of blank characters for indentation of the next item.

Other features are:
- Attributes to the 'startElement' method are passed as keyword arguments.
- The extension class maintains an internal stack of current element tags,
  obviating the need to specify tag name in method 'endElement'. This
  simplifies XML generation slightly for application code.
- A number of common attribute keys are automatically translated from
  the form 'xxx_yyy' to 'xxx:yyy', to simplify giving attributes
  as keyword arguments.
- The class 'Xml' to build lightweight element trees for output with
  XMLIndentGenerator.

The prefix mapping and NS methods have not yet been properly implemented.

Example usage:

    from xmlindent import XMLIndentGenerator
    gen = XMLIndentGenerator()
    gen.startDocument()
    gen.startElement('root', attr1='1', attr2='text&data')
    gen.newline()
    gen.startElement('item')
    xml = Xml('another', Xml('and-another'), id='2', color='grey')
    xml.output(gen)
    gen.newline()
    gen.startElement('item2')
    gen.characters('stuff')
    gen.endElement()
    gen.newline()
    gen.endElement()
    gen.newline()
    gen.endElement()
    gen.endDocument()

This is the resulting XML:

<?xml version="1.0" encoding="iso-8859-1"?>
<root attr2="text&amp;data" attr1="1">
  <item>
    <another color="grey" id="2">
      <and-another/>
    </another>
    <item2>stuff</item2>
  </item>
</root>

------------------------------------------------------------
Version 1.2

2004-12-09  split out from other code
2007-08-23  cleanup, docs written
2007-08-31  added Xml

Copyright (C) 2007 Per Kraulis

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

__version__ = '1.2'

from xml.sax.saxutils import XMLGenerator, quoteattr, escape


class XMLIndentGenerator(XMLGenerator):
    """This extension class provides features to generate indented XML.
    Attributes to startElement are passed as keyword arguments.
    It also maintains an internal stack of current element tags, thus
    obviating the need to specify the element tag in endElement."""

    attr_name_aliases = dict(xml_base='xml:base',
                             xmlns_xlink='xmlns:xlink',
                             xlink_type='xlink:type',
                             xlink_href='xlink:href',
                             xlink_title='xlink:title',
                             xlink_from='xlink:from',
                             xlink_to='xlink:to',
                             xlink_label='xlink:label',
                             xlink_arcrole='xlink:arcrole',
                             xlink_show='xlink:show',
                             xlink_actuate='xlink:actuate',
                             href_template='href-template')

    def __init__(self, out=None, encoding="iso-8859-1", increment=2):
        """out        Opened output file; sys.stdout if None given.
        encoding   Character encoding to use.
        increment  Indentation increment to use.
        """
        XMLGenerator.__init__(self, out=out, encoding=encoding)
        self._element_stack = []
        self._in_cdata = False
        self._indent_increment = increment
        self._newline_flag = False
        self._indent_level = 0

    def endDocument(self):
        """Finish document output; checks that the element tag stack is empty."""
        if len(self._element_stack) != 0:
            raise IOError("element(s) not closed: %s"
                          % str(self._element_stack))
        XMLGenerator.endDocument(self)

    def startElement(self, tagname, forceNewLine=True, **attrs):
        """Start a new element, with appropriate indentation."""
        if not self._newline_flag:
            self.ignorableWhitespace(' ' * self._indent_level)
        XMLGenerator.startElement(self, tagname,
                                  self.translate_name_aliases(**attrs))
        self._element_stack.append(tagname)
        self._newline_flag = True
        self._indent_level += self._indent_increment
	if forceNewLine:
		self.newline()

    def endElement(self, tagname=None, forceNewLine=True):
        """Finish the current element.
        The tagname argument is not required and is in fact currently ignored,
        since the internal element tagname stack is used."""
        self._indent_level -= self._indent_increment
        if not self._newline_flag:
            self.ignorableWhitespace(' ' * self._indent_level)
        XMLGenerator.endElement(self, self._element_stack.pop())
        self._newline_flag = True
	if forceNewLine:
		self.newline()

    def emptyElement(self, tagname, forceNewLine=True, **attrs):
        """Output an element with no content, with appropriate indentation."""
        if not self._newline_flag:
            self.ignorableWhitespace(' ' * self._indent_level)
        self._out.write('<' + tagname)
        for (name, value) in attrs.items():
            name = self.attr_name_aliases.get(name, name)
            self._out.write(' %s=%s' % (name, quoteattr(value)))
        self._out.write('/>')
        self._newline_flag = True
	if forceNewLine:
		self.newline()

    def characters(self, content, forceNewLine=True):
        """Output the given character content."""
        if self._in_cdata:
            self._out.write(content.replace(']]>', ']]>]]&gt;<![CDATA['))
        else:
            self._out.write(escape(content))
	if forceNewLine:
		self.newline()

    def comment(self, content):
        """Output an XML comment."""
        self._out.write('<!--')
        self._out.write(content)
        self._out.write('-->')

    def startCDATA(self):
        """Start a CDATA block of characters. The application must finish
        the CDATA block explicitly. There is currently no internal check
        that this is done properly."""
        self._out.write('<![CDATA[')
        self._in_cdata = True

    def endCDATA(self):
        """Finish a CDATA block of characters."""
        self._out.write(']]>')
        self._in_cdata = False

    def newline(self, force=False):
        """Output a newline character. 
	If force, then do so even if the previous character was a newline."""
        if self._newline_flag or force:
            self.ignorableWhitespace('\n')
        self._newline_flag = False

    def translate_name_aliases(cls, **attrs):
        new = {}
        for key, value in attrs.items():
            new[cls.attr_name_aliases.get(key, key)] = value
        return new
    translate_name_aliases = classmethod(translate_name_aliases)

    def writeTextElement(self, tagname, text, **attrs):
	    self.startElement(tagname, False, **attrs)
	    self.characters(text, False)
	    self.endElement()


class Xml(object):
    """Light-weight XML element node, for output via XMLIndentGenerator."""

    def __init__(self, tagname, *data, **attrs):
        """tag    tagname of this element
        data   Xml instances or strings that are the content of this element
        attrs  attributes of this element"""
        self.tagname = tagname
        self.data = data
        self.attrs = attrs.copy()
        self.newline = self.attrs.pop('newline', True)

    def output(self, gen):
        """Output the contents of this Xml using the given generator."""
        if self.newline: gen.newline()
        if self.data:
            gen.startElement(self.tagname, **self.attrs)
            for data in self.data:
                if isinstance(data, Xml):
                    data.output(gen)
                    newline = True      # indent for nicer output
                else:                   # string, or other instance
                    gen.characters(str(data))
                    newline = False     # avoid adding blanks to string
            if newline:                 # only if previous was end of element
                gen.newline()
            gen.endElement()
        else:
            gen.emptyElement(self.tagname, **self.attrs)


if __name__ == '__main__':
    gen = XMLIndentGenerator()
    gen.startDocument()
    gen.startElement('root', attr1='1', attr2='text&data')
    gen.newline()
    gen.startElement('item')
    xml = Xml('another', Xml('and-another'), id='2', color='grey')
    xml.output(gen)
    gen.newline()
    gen.startElement('item2')
    gen.characters('stuff')
    gen.endElement()
    gen.newline()
    gen.endElement()
    gen.newline()
    gen.endElement()
    gen.endDocument()
