package de.tudresden.sumo.test;

import static org.junit.Assert.assertEquals;

import org.junit.Test;

import de.tudresden.sumo.objects.SumoGeometry;

public class SimpleTest {

	public static void main(String[] args) {

		SimpleTest s = new SimpleTest();
		s.validate();
		
	}	
	
	@Test
	public void validate() {
	
		
		StringBuilder sb = new StringBuilder();
		Number size = Math.min(1, Math.random() * 100);
		
		for(int i=0; i<size.intValue(); i++) {
			sb.append(i+","+i+" ");
		}
		
		SumoGeometry sg = new SumoGeometry();
		sg.fromString(sb.toString().trim());
		
		assertEquals(sg.coords.size(), size.intValue());		
		
	}
	
}
