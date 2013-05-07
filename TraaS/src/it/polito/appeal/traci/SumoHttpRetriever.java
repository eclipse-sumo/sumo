/*   
    Copyright (C) 2011 ApPeAL Group, Politecnico di Torino

    This file is part of TraCI4J.

    TraCI4J is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TraCI4J is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TraCI4J.  If not, see <http://www.gnu.org/licenses/>.
*/

package it.polito.appeal.traci;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.helpers.XMLReaderFactory;

/**
 * Downloads a SUMO config file from an HTTP URL, parses its input file
 * references and downloads them too. All the resulting contents will be stored
 * as temporary files.
 * <p>
 * TODO: support for relative paths (this needs rewriting the config file)
 * @author Enrico Gueli &lt;enrico.gueli@polito.it&gt;
 *
 */
public class SumoHttpRetriever {
	
	private URL configURL;
	private File tempDataDir;
	private File configFile;

	public SumoHttpRetriever(String configURL) throws MalformedURLException {
		this.configURL = new URL(configURL);
	}

	public void fetchAndParse() throws IOException, SAXException {
		tempDataDir = createTempDirectory("sumo");
		
		String configURLStr = configURL.toString();
		int lastPathSep = configURLStr.lastIndexOf('/');
		String baseAddr = configURLStr.substring(0, lastPathSep+1);
		String configFileName = configURLStr.substring(lastPathSep+1, configURLStr.length());

		configFile = new File(tempDataDir + File.separator + configFileName);
		downloadTo(configURL, configFile);
		
		ConfigFileContentHandler configHandler = new ConfigFileContentHandler();
		XMLReader xmlReader = XMLReaderFactory.createXMLReader("org.apache.xerces.parsers.SAXParser");
		xmlReader.setContentHandler(configHandler);
		xmlReader.parse(new InputSource(new FileInputStream(configFile)));
		

		Set<String> inputFiles = configHandler.getInputFiles();
		for (String inputFileName : inputFiles) {
			if (inputFileName.contains("/") || inputFileName.contains("\\"))
				throw new UnsupportedOperationException("path separators are not supported yet");
			
			File inputFile = new File(tempDataDir + File.separator + inputFileName);
			URL inputURL = new URL(baseAddr + inputFileName);
			downloadTo(inputURL, inputFile);
		}
	}

	private void downloadTo(URL url, File out) throws IOException {
		URLConnection conn = url.openConnection();
		BufferedInputStream is = new BufferedInputStream(conn.getInputStream());
		BufferedOutputStream os = new BufferedOutputStream(new FileOutputStream(out));

		final int BLOCK_SIZE = 1024;
		byte[] buf = new byte[BLOCK_SIZE];
		int size;
		while((size = is.read(buf, 0, BLOCK_SIZE)) > 0)
			os.write(buf, 0, size);
		
		is.close();
		os.close();
	}

	public String getConfigFileName() {
		return configFile.toString();
	}
	
	public void close() {
		for (File file : tempDataDir.listFiles()) {
			file.delete();
		}
		tempDataDir.delete();
	}
	
	
	public static File createTempDirectory(String prefix) throws IOException {
		final File temp;

		temp = File.createTempFile(prefix, Long.toString(System.nanoTime()));

		if(!(temp.delete()))
		{
			throw new IOException("Could not delete temp file: " + temp.getAbsolutePath());
		}

		if(!(temp.mkdir()))
		{
			throw new IOException("Could not create temp directory: " + temp.getAbsolutePath());
		}

		return (temp);
	}

}

class ConfigFileContentHandler extends DefaultHandler {

	private static final String INPUT_ELEMENT = "input";
	private static final String[] INPUT_FILE_ELEMENTS = new String[] {
		"net-file",
		"route-files",
		"additional-files",
		"weight-files"
	};

	private boolean readingInput = false;
	private Set<String> inputFiles = new HashSet<String>();
	
	
	@Override
	public void startElement(String uri, String localName, String qName,
			Attributes atts) throws SAXException {
		
		if (!readingInput) {
			if (localName.equals(INPUT_ELEMENT))
				readingInput = true;
		}
		else {
			for (String inputElement : INPUT_FILE_ELEMENTS) {
				if (inputElement.equals(localName)) {
					String fileNamesStr = atts.getValue("", "value");
					if (!fileNamesStr.isEmpty()) {
						String[] fileNames = fileNamesStr.split(",");
						Collections.addAll(inputFiles, fileNames);
					}
				}
			}
		}
	}

	@Override
	public void endElement(String uri, String localName, String qName)
	throws SAXException {
		if (readingInput && localName.equals(INPUT_ELEMENT))
			readingInput = false;
	}

	public Set<String> getInputFiles() {
		return inputFiles;
	}

}