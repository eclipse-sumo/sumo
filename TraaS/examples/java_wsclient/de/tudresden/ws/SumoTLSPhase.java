
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for sumoTLSPhase complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="sumoTLSPhase">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="duration" type="{http://www.w3.org/2001/XMLSchema}int"/>
 *         &lt;element name="duration1" type="{http://www.w3.org/2001/XMLSchema}int"/>
 *         &lt;element name="duration2" type="{http://www.w3.org/2001/XMLSchema}int"/>
 *         &lt;element name="phasedef" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "sumoTLSPhase", propOrder = {
    "duration",
    "duration1",
    "duration2",
    "phasedef"
})
public class SumoTLSPhase {

    protected int duration;
    protected int duration1;
    protected int duration2;
    protected String phasedef;

    /**
     * Gets the value of the duration property.
     * 
     */
    public int getDuration() {
        return duration;
    }

    /**
     * Sets the value of the duration property.
     * 
     */
    public void setDuration(int value) {
        this.duration = value;
    }

    /**
     * Gets the value of the duration1 property.
     * 
     */
    public int getDuration1() {
        return duration1;
    }

    /**
     * Sets the value of the duration1 property.
     * 
     */
    public void setDuration1(int value) {
        this.duration1 = value;
    }

    /**
     * Gets the value of the duration2 property.
     * 
     */
    public int getDuration2() {
        return duration2;
    }

    /**
     * Sets the value of the duration2 property.
     * 
     */
    public void setDuration2(int value) {
        this.duration2 = value;
    }

    /**
     * Gets the value of the phasedef property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getPhasedef() {
        return phasedef;
    }

    /**
     * Sets the value of the phasedef property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setPhasedef(String value) {
        this.phasedef = value;
    }

}
