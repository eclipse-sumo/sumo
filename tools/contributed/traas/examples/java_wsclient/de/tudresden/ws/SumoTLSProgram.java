
package de.tudresden.ws;

import java.util.ArrayList;
import java.util.List;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java-Klasse für sumoTLSProgram complex type.
 * 
 * <p>Das folgende Schemafragment gibt den erwarteten Content an, der in dieser Klasse enthalten ist.
 * 
 * <pre>
 * &lt;complexType name="sumoTLSProgram">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="subID" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="type" type="{http://www.w3.org/2001/XMLSchema}int"/>
 *         &lt;element name="subParameter" type="{http://www.w3.org/2001/XMLSchema}int"/>
 *         &lt;element name="currentPhaseIndex" type="{http://www.w3.org/2001/XMLSchema}int"/>
 *         &lt;element name="phases" type="{http://ws.tudresden.de/}sumoTLSPhase" maxOccurs="unbounded" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "sumoTLSProgram", propOrder = {
    "subID",
    "type",
    "subParameter",
    "currentPhaseIndex",
    "phases"
})
public class SumoTLSProgram {

    protected String subID;
    protected int type;
    protected int subParameter;
    protected int currentPhaseIndex;
    @XmlElement(nillable = true)
    protected List<SumoTLSPhase> phases;

    /**
     * Ruft den Wert der subID-Eigenschaft ab.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    public String getSubID() {
        return subID;
    }

    /**
     * Legt den Wert der subID-Eigenschaft fest.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setSubID(String value) {
        this.subID = value;
    }

    /**
     * Ruft den Wert der type-Eigenschaft ab.
     * 
     */
    public int getType() {
        return type;
    }

    /**
     * Legt den Wert der type-Eigenschaft fest.
     * 
     */
    public void setType(int value) {
        this.type = value;
    }

    /**
     * Ruft den Wert der subParameter-Eigenschaft ab.
     * 
     */
    public int getSubParameter() {
        return subParameter;
    }

    /**
     * Legt den Wert der subParameter-Eigenschaft fest.
     * 
     */
    public void setSubParameter(int value) {
        this.subParameter = value;
    }

    /**
     * Ruft den Wert der currentPhaseIndex-Eigenschaft ab.
     * 
     */
    public int getCurrentPhaseIndex() {
        return currentPhaseIndex;
    }

    /**
     * Legt den Wert der currentPhaseIndex-Eigenschaft fest.
     * 
     */
    public void setCurrentPhaseIndex(int value) {
        this.currentPhaseIndex = value;
    }

    /**
     * Gets the value of the phases property.
     * 
     * <p>
     * This accessor method returns a reference to the live list,
     * not a snapshot. Therefore any modification you make to the
     * returned list will be present inside the JAXB object.
     * This is why there is not a <CODE>set</CODE> method for the phases property.
     * 
     * <p>
     * For example, to add a new item, do as follows:
     * <pre>
     *    getPhases().add(newItem);
     * </pre>
     * 
     * 
     * <p>
     * Objects of the following type(s) are allowed in the list
     * {@link SumoTLSPhase }
     * 
     * 
     */
    public List<SumoTLSPhase> getPhases() {
        if (phases == null) {
            phases = new ArrayList<SumoTLSPhase>();
        }
        return this.phases;
    }

}
