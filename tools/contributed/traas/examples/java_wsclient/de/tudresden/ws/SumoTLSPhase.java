
package de.tudresden.ws;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java-Klasse für sumoTLSPhase complex type.
 * 
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
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
     * Ruft den Wert der duration-Eigenschaft ab.
     * 
     */
    public int getDuration() {
        return duration;
    }

    /**
     * Legt den Wert der duration-Eigenschaft fest.
     * 
     */
    public void setDuration(int value) {
        this.duration = value;
    }

    /**
     * Ruft den Wert der duration1-Eigenschaft ab.
     * 
     */
    public int getDuration1() {
        return duration1;
    }

    /**
     * Legt den Wert der duration1-Eigenschaft fest.
     * 
     */
    public void setDuration1(int value) {
        this.duration1 = value;
    }

    /**
     * Ruft den Wert der duration2-Eigenschaft ab.
     * 
     */
    public int getDuration2() {
        return duration2;
    }

    /**
     * Legt den Wert der duration2-Eigenschaft fest.
     * 
     */
    public void setDuration2(int value) {
        this.duration2 = value;
    }

    /**
     * Ruft den Wert der phasedef-Eigenschaft ab.
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
     * Legt den Wert der phasedef-Eigenschaft fest.
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
