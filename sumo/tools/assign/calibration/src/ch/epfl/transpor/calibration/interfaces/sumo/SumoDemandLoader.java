/*
 * Do not distribute this software. It is property of
 * 
 *   Gunnar Flötteröd
 *   Transport and Mobility Laboratory
 *   Ecole Polytechnique Fédérale de Lausanne
 *
 * Please mention explicitly the use of this software when publishing results.
 *
 * This software is provided WITHOUT ANY WARRANTY; without even the implied 
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. Nor myself
 * neither my employer (EPFL) shall be liable for any damage of any kind in 
 * connection with the use of this software.
 * 
 */
package ch.epfl.transpor.calibration.interfaces.sumo;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.xml.sax.Attributes;
import org.xml.sax.helpers.DefaultHandler;

import ch.epfl.transpor.calibration.demand.PlanFactory;

/**
 * 
 * @author Gunnar Flötteröd
 *
 */
class SumoDemandLoader extends DefaultHandler {

	// -------------------- CONSTANTS --------------------

	static final String VEHICLE_ELEM = "vehicle";

	static final String VEHICLEID_ATTR = "id";

	static final String DEPART_ATTR = "depart";

	static final String ROUTE_ELEM = "route";

	static final String ROUTEID_ATTR = "id";

	static final String EDGES_ATTR = "edges";

	static final String EXITTIMES_ATTR = "exitTimes";

	static final String CHOICEPROB_ATTR = "probability";

	// -------------------- MEMBERS --------------------

	private final PlanFactory<String> planFact = new PlanFactory<String>();

	private SumoPopulation tempPop = null;

	private SumoAgent tempAgent = null;

	// -------------------- CONSTRUCTION --------------------

	SumoDemandLoader() {
	}

	// -------------------- PARSING --------------------

	SumoPopulation load(final String fileName) {
		this.tempPop = new SumoPopulation();
		try {
			final SAXParserFactory factory = SAXParserFactory.newInstance();
			final SAXParser saxParser = factory.newSAXParser();
			saxParser.parse(fileName, this);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return this.tempPop;
	}

	private int parseTime_s(final String exitStr_s) {
		return (int) Math.round(Double.parseDouble(exitStr_s));
	}

	@Override
	public void startElement(String namespaceURI, String sName, String qName,
			Attributes attrs) {
		if (VEHICLE_ELEM.equals(qName)) {
			final String agentId = attrs.getValue(VEHICLEID_ATTR);
			final String dpt_s = attrs.getValue(DEPART_ATTR);
			this.tempAgent = new SumoAgent(agentId, this.parseTime_s(dpt_s));
			this.tempPop.addAgent(tempAgent);
		} else if (ROUTE_ELEM.equals(qName)) {
			final String routeId = attrs.getValue(ROUTEID_ATTR);
			final String[] edges = attrs.getValue(EDGES_ATTR).split("\\s");
			final String[] exits = attrs.getValue(EXITTIMES_ATTR).split("\\s");
			final String prob = attrs.getValue(CHOICEPROB_ATTR);
			this.planFact.reset();
			if (edges.length > 0 && !"".equals(edges[0])) {
				this.planFact.addEntry(edges[0], this.tempAgent.getDptTime_s());
				for (int i = 1; i < edges.length; i++) {
					this.planFact.addTurn(edges[i], this
							.parseTime_s(exits[i - 1]));
				}
				this.planFact
						.addExit(this.parseTime_s(exits[exits.length - 1]));
			}
			this.tempAgent.addSumoPlan(new SumoPlan(routeId, this.planFact
					.getResult(), Double.parseDouble(prob)));
		}
	}
}