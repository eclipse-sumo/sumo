// Graph.cpp: Implementierung der Klasse Graph.
//
//////////////////////////////////////////////////////////////////////

#include "Graph.h"
#include "time.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

using namespace std;
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

Graph::Graph(){}

void
Graph::AddVertexByXY(int x, int y)
{

    Vertex* ptemp=new Vertex(x,y);
    vArray.push_back(ptemp);
}

//Sucht den Knoten mit den Koodinatenten x,y in dem Knotenarray//
Vertex* Graph::SearchVertex(int x, int y)
{
    Vertex* rueckgabe=NULL;
    Vertex* ptemp;
    unsigned int k;
    for (k=0 ; k<vArray.size() ; k++)
    {
        ptemp = vArray[k];
        if ((x==ptemp->GetX())&&(y==ptemp->GetY()))
        {
            rueckgabe=ptemp;
        }

    }
    return rueckgabe;
}

//Entfernt den Knoten mitsamt aller inzidenten Kanten/
void
Graph::DelVertex(int x, int y)
{
    Vertex* ptemp;
    unsigned int k;

    for(k=0; k<vArray.size(); k++)
    {
        //Suchen des zu löschenden Knotens
        ptemp=vArray[k];

        if((x==ptemp->GetX())&&(y==ptemp->GetY()))
        {
            int nf= ptemp->GetNachfolger();

            //Nur für Anzahl der Nachfolger/Vorgänger = 2 impl.!!!//
            if(nf==2)
            {
                Vertex* neuV = ptemp->GetNachfolgeVertex(0);
                Vertex* neuW = ptemp->GetNachfolgeVertex(1);

                DelEdge(ptemp,neuV);
                DelEdge(ptemp,neuW);
                DelEdge(neuV,ptemp);
                DelEdge(neuW,ptemp);

                AddEdgeByVertex(neuV, neuW);
                AddEdgeByVertex(neuW, neuV);

                for (int z=0;z<neuV->GetNachfolger();z++)
                {
                    if (neuV->GetNachfolgeVertex(z)==ptemp)
                    {
                        neuV->DelNachfolger(z);
                    }
                }

                for (int m=0;m<neuW->GetNachfolger();m++)
                {
                    if (neuW->GetNachfolgeVertex(m)==ptemp)
                    {
                        neuW->DelNachfolger(m);
                    }
                }
            }

            ptemp->DelInzidentEdge();
            ptemp->DelNachfolgeArray();
            ptemp->DelVorgaengerArray();
            vArray.erase(vArray.begin()+k);
        }
    }
}
///////////////Kantenmethoden//////////////////////

void
Graph::AddEdgeByVertex(Vertex* v, Vertex* w)
{
    Edge* e = new Edge(v,w);
    eArray.push_back(e);
    v->AddNachfolger(w);
    w->AddVorgaenger(v);
    v->InkrementOutDegree();
    w->InkrementInDegree();
}

Edge* Graph::SearchEdge(Vertex* v, Vertex* w)
{
    Edge* rueckgabe=NULL;
    Edge* ptemp;
    unsigned int i;

    for(i=0; i<eArray.size();i++)
    {
        ptemp=eArray[i];
        if((ptemp->GetStartingVertex()==v)&&(ptemp->GetEndingVertex()==w)){
            rueckgabe=ptemp;
        }

    }
    return rueckgabe;
}

vector<Vertex*> Graph::GetVArray()
{
    return vArray;
}

vector<Vertex*> Graph::GetPfadArray()
{
    return pfad;
}

vector<Edge*> Graph::GetEArray()
{
    return eArray;
}

/* Noch zu impl.!!!
Graph::PushVertex(int index,int xNew, int yNew){
    Vertex temp= vArray.Item(index);
    temp.SetX(xNew);
    temp.SetY(yNew);
    vArray.Item(index)=temp;
}
*/

//
//einige Hilfsmethoden
//

//liefert die Anzahl der Elemente des Knotenarrays zurück
int Graph::Number_of_Vertex()
{
    return vArray.size();
}

//liefert die Anzahl der Elemente des Edgearrays zurück
int Graph::Number_of_Edges()
{
    return eArray.size();
}

//liefert Element des Knotenarrays mit Index i zurück
Vertex* Graph::GetVertex(int i)
{
    return vArray[i];
}

Edge* Graph::GetEdge(int i)
{
        return eArray[i];
}

void
Graph::drive(int i)
{
    pfad.clear();
    int driveRandom;
    Vertex* aktuell=NULL;
    Vertex* besucht=NULL;
    driveRandom=rand()%vArray.size();
    aktuell=GetVertex(driveRandom);

    for(int m=0; m<i; m++)
    {
        int nachfolger=aktuell->GetNachfolger();
        int myrandom=rand()%nachfolger;
        pfad.push_back(aktuell);
        if(nachfolger==1)
        {
            besucht=aktuell;
            aktuell=aktuell->GetNachfolgeVertex(0);
            continue;
        }
        while(besucht==aktuell->GetNachfolgeVertex(myrandom))
        {
            myrandom=rand()%nachfolger;
        }
        besucht=aktuell;
        aktuell=aktuell->GetNachfolgeVertex(myrandom);
    }
}

int Graph::GetPfad()
{
    return pfad.size();
}


void
Graph::Reduce()
{
    Vertex* ptemp_null;
    Vertex* ptemp_eins;
    Vertex* paktuell;
    double abstand_pn_pa,abstand_pa_pe,abstand_pn_pe,delta_x1,delta_x2,delta_x3,delta_y1,delta_y2,delta_y3,abweichung;
    int x1,x2,x3,y1,y2,y3,m,knotenanz;
    unsigned int n;
    n=0;
    //Erlaubte Abweichung
    abweichung=0.003;
    knotenanz=vArray.size();
    for (m=0;m<knotenanz;m++)
    {
        n=0;
        while (n<vArray.size())
        {
            //Hole Knoten an Stelle "n" im Knotenarray
            paktuell=vArray[n];
            //Betrachte nur Nicht-Kreuzungsknoten
            int grad=paktuell->GetDegree();

            if (grad==4)
            {
                //Hole Pointer auf Nachbarknoten
                ptemp_null=paktuell->GetNachfolgeVertex(0);
                ptemp_eins=paktuell->GetNachfolgeVertex(1);

                //Hole alle Koordinaten
                x1=ptemp_null->GetX();
                x2=paktuell->GetX();
                x3=ptemp_eins->GetX();
                y1=ptemp_null->GetY();
                y2=paktuell->GetY();
                y3=ptemp_eins->GetY();

                //Berechne alle koordinatenweisen Abstände der Punkte zum Quadrat
                delta_x1=(x1-x2)*(x1-x2);
                delta_x2=(x2-x3)*(x2-x3);
                delta_x3=(x1-x3)*(x1-x3);
                delta_y1=(y1-y2)*(y1-y2);
                delta_y2=(y2-y3)*(y2-y3);
                delta_y3=(y1-y3)*(y1-y3);

                //Berechne die euklidischen Abstände der Punkte
                abstand_pn_pa=sqrt(delta_x1+delta_y1);
                abstand_pa_pe=sqrt(delta_x2+delta_y2);
                abstand_pn_pe=sqrt(delta_x3+delta_y3);

                //Ist Abstand von Punkt eins plus Abstand von Punkt zwei größer als
                //Abstand von Punkt eins zu Punkt drei (inclusive Abweichung),
                //dann kann der Punkt nicht entfernt werden
                if (fabs(abstand_pn_pa+abstand_pa_pe)<fabs(abstand_pn_pe)*(1.0+abweichung))
                {
                    DelVertex(paktuell->GetX(),paktuell->GetY());
                    n++;
                    break;
                }
            }
            n++;
        }
    }
}

void
Graph::Reduce_plus()
{
    double toleranz=7;
    int i;
    int m=0;
    unsigned int n=0;
    double delta_x=0;
    double delta_y=0;
    double distanz_a=0;
    double distanz_b=0;
    double quotient =0;

    int delta_x1=0;
    int delta_x2=0;
    int delta_y1=0;
    int delta_y2=0;

    //Vertex_Array myArray=gr.GetVArray();

    i = Number_of_Vertex();

    for (n=0;n<vArray.size();n++)
    {
        Vertex* paktuell=vArray[n];
        int grad=paktuell->GetDegree();

        if (grad==4)
        {
            Vertex* ptemp_null=paktuell->GetNachfolgeVertex(0);
            Vertex* ptemp_eins=paktuell->GetNachfolgeVertex(1);

            delta_x1=ptemp_null->GetX()-paktuell->GetX();
            delta_x2=paktuell->GetX()-ptemp_eins->GetX();
            delta_y1=ptemp_null->GetY()-paktuell->GetY();
            delta_y2=paktuell->GetY()-ptemp_eins->GetY();
            distanz_a=sqrt((double) (delta_x1*delta_x1+delta_y1*delta_y1));
            distanz_b=sqrt((double) (delta_x2*delta_x2+delta_y2*delta_y2));

            if(distanz_a<distanz_b)
            {
                quotient=(distanz_b/distanz_a);
                delta_x1=delta_x1*quotient;
                delta_y1=delta_y1*quotient;
            }

            else
            {
                quotient=(distanz_a/distanz_b);
                delta_x2=delta_x2*quotient;
                delta_y2=delta_y2*quotient;
            }

            delta_x=delta_x1-delta_x2;
            delta_y=delta_y1-delta_y2;

            if (((delta_x<toleranz)&(delta_y<toleranz))&((delta_x>-toleranz)&(delta_y>-toleranz)))
            {
                DelVertex(paktuell->GetX(),paktuell->GetY());
                n--;
            } 
		}
        n++;
	}
}


///////////////////NEU!!!!//////////////////////////////
void
Graph::DelEdge(Vertex* v,Vertex* w)
{
    Edge* ptemp;
    unsigned int i;

    for(i=0; i<eArray.size();i++)
    {
        ptemp=eArray[i];
        if((ptemp->GetStartingVertex()==v)&&(ptemp->GetEndingVertex()==w))
        {
            for(int k=0; k<v->GetNachfolger();k++)
            {
                Vertex* vtemp=v->GetNachfolgeVertex(k);
                if(vtemp==w)
                {
                    v->DelNachfolger(k);
                }
            }

            for(int l=0; l<w->GetVorgaenger();l++)
            {
                Vertex* vtemp1=w->GetVorgaengerVertex(l);
                if(vtemp1==v)
                {
                    w->DelVorgaenger(l);
                }
            }


            eArray.erase(eArray.begin()+i);
            v->DekrementOutDegree();
            w->DekrementInDegree();

        }
    }
}

void
Graph::DelDoubleEdge(Vertex* v, Vertex* w)
{
    Edge* ptemp;
	unsigned int i;
	
	for(i=0; i<eArray.size();i++)
	{
			ptemp=eArray[i];
			if((ptemp->GetStartingVertex()==v)&&(ptemp->GetEndingVertex()==w))
			{
				eArray.erase(eArray.begin()+i);
				v->DekrementOutDegree();
				w->DekrementInDegree();
				i--;
			}
	}
}

void
Graph::Reduce_Edges()
{
    Edge* etemp;
    Edge* etemp1;
    Vertex* start;
    Vertex* end;
    Vertex* start1;
    Vertex* end1;

    for(long i=0; i<eArray.size();i++)
    {
        etemp=eArray[i];
        start=etemp->GetStartingVertex();
        end=etemp->GetEndingVertex();

        for(long j=0; j<eArray.size() ; j++)
        {
            etemp1=eArray[j];
            start1=etemp1->GetStartingVertex();
            end1=etemp1->GetEndingVertex();

            if(start==end)
            {
                DelDoubleEdge(start,end);
                break;
            }
        }
    }
}
/////////////////////////ENDE NEU!!!!!!!!!!!!!!////////////////////

void
Graph::Export_Vertexes_XML()
{
    string append;
    string append1;
    string append2;
    string help="";

    append="<nodes>\n";
    append2="</nodes>\n";

    const char* app=append.c_str();
    const char* app2=append2.c_str();
    const char* app1;
    const char* mychar;

    Vertex* temp;
    FILE* vfile= fopen("export.nod.xml","w");
    int Vertices= vArray.size();
    fputs(app,vfile);
    int i=0;

    for(i ; i<Vertices; i++)
    {
        append1=help;
        temp=vArray[i];
        int x=temp->GetX();
        int y=temp->GetY();
        append1="<node id=\"";
        app1=append1.c_str();
        fputs(app1,vfile);
        mychar=inttostr(i);
        fputs(mychar,vfile);
        append1="\" x=\"";
        app1=append1.c_str();
        fputs(app1,vfile);
        mychar=inttostr(x);
        fputs(mychar,vfile);
        append1=".0\" y=\"";
        app1=append1.c_str();
        fputs(app1,vfile);
        mychar=inttostr(y);
        fputs(mychar,vfile);
        append1=".0\" type=\"priority\"/>\n";
        app1=append1.c_str();
        fputs(app1,vfile);
    }
    fputs(app2, vfile);
    fclose(vfile);
}

void
Graph::Export_Edges_XML()
{
    string append;
    string append1;
    string append2;
    string help="";
    append="<edges>\n";
    append2="</edges>\n";
    const char* app=append.c_str();
    const char* app1;
    const char* app2=append2.c_str();
    const char* mychar;
    Edge* temp;
    Vertex* start;
    Vertex* end;
    int index_start=0;
    int index_end=0;

    FILE* efile= fopen("export.edg.xml","w");
    int Edges= eArray.size();
    fputs(app,efile);

    for(int i=0; i<Edges; i++)
    {
        append1=help;
        temp=eArray[i];
        start=temp->GetStartingVertex();
        end=temp->GetEndingVertex();
        index_start=GetIndex(start);
        index_end=GetIndex(end);
        append1="<edge id=\"";
        app1=append1.c_str();
        fputs(app1,efile);
        mychar=inttostr(i);
        fputs(mychar,efile);
        append1="\" fromnode=\"";
        app1=append1.c_str();
        fputs(app1,efile);
        mychar=inttostr(index_start);
        fputs(mychar,efile);
        append1="\" tonode=\"";
        app1=append1.c_str();
        fputs(app1,efile);
        mychar=inttostr(index_end);
        fputs(mychar,efile);
        append1="\" priority=\"78\" nolanes=\"1\" speed=\"50.000\"/>\n";
        app1=append1.c_str();
        fputs(app1,efile);

    }
    fputs(app2, efile);
    fclose(efile);
}

char* Graph::inttostr(int i)
{
    char pch[10]="";
    return itoa(i,pch,10);
}

char* Graph::doubletostr(double i,int count)
{
	int dec;
	int sig;
	char* buffer = _ecvt(i,count,&dec,&sig);
		
	return (buffer);
}

/*Liefert den Index eines Knotenobjekts*/
int Graph::GetIndex(Vertex* v)
{
    Vertex* temp;
    int index=-1;
    for (int i=0; i<vArray.size(); i++)
    {
        temp=vArray[i];
        if (v==temp) index=i;
    }
    return index;
}

Graph::GetTraces(int cars, int fuel)
{
	string append;
	
	const char* mychar;
	const char* app;
		
	time_t rawtime;
	tm* ptm;
	
	FILE* Traces = fopen("Traces1.txt","w");
		
		drive(fuel);
		pfad = GetPfadArray();
		unsigned int lauf=1;
	
		while(lauf <= pfad.size())
		{
			Vertex* ptemp=pfad[lauf-1];
			//ohne malmethoden//

			time(&rawtime);
			ptm=gmtime(&rawtime);

			double mylat=ptemp->GetGPSLat();
			double mylon=ptemp->GetGPSLon();

			/****So soll es iterativ aussehen****/

			
			 

			append="$GPRMC,1";
			app = append.c_str();
			fputs(app,Traces);

			/*
			holen der aktuellen Zeit
			*/
			mychar = inttostr(ptm->tm_hour+1);
			fputs(mychar,Traces);
		
			mychar = inttostr(ptm->tm_min);
			fputs(mychar,Traces);
			
			mychar = inttostr(ptm->tm_sec);
			fputs(mychar,Traces);
			
			
			
			append=",A,";
			app=append.c_str();
			fputs(app,Traces);
			
			
			mychar=inttostr((int)(mylat*100));
			fputs(mychar,Traces);
						
			append=".";
			app=append.c_str();
			fputs(app,Traces);

			mychar=doubletostr(mylat*100-(int)(mylat*100),6);
			fputs(mychar,Traces);

			append=",N,00";
			app=append.c_str();
			fputs(app,Traces);

				
			mychar = inttostr((int)(mylon*100));
			fputs(mychar,Traces);
			
			append=".";
			app=append.c_str();
			fputs(app,Traces);
			
			mychar = doubletostr(mylon*100-(int)(mylon*100),6);
			fputs(mychar,Traces);
			// $ GPRMC,201512,A,1234.23456,N,1234.1234,
			append=",E,,,";
			app=append.c_str();
			fputs(app,Traces);
			
		

			/*
			Holen des aktuellen Datums
			*/

			mychar = inttostr(ptm->tm_mday);
			fputs(mychar,Traces);
			mychar = inttostr(ptm->tm_mon+1);
			fputs(mychar,Traces);
			mychar = inttostr(ptm->tm_year+1900);
			fputs(mychar,Traces);

			append=",,\n";
			app=append.c_str();
			fputs(app,Traces);

			// Jetzt das ganze für $GPGGA

			append="$GPGGA,1";
			app = append.c_str();
			fputs(app,Traces);

			/*
			holen der aktuellen Zeit
			*/
			mychar = inttostr(ptm->tm_hour+1);
			fputs(mychar,Traces);
		
			mychar = inttostr(ptm->tm_min);
			fputs(mychar,Traces);
			
			mychar = inttostr(ptm->tm_sec);
			fputs(mychar,Traces);
			
			
			
			append=",";
			app=append.c_str();
			fputs(app,Traces);
			
			
			mychar=inttostr((int)(mylat*100));
			fputs(mychar,Traces);
						
			append=".";
			app=append.c_str();
			fputs(app,Traces);

			mychar=doubletostr(mylat*100-(int)(mylat*100),6);
			fputs(mychar,Traces);

			append=",N,00";
			app=append.c_str();
			fputs(app,Traces);

				
			mychar = inttostr((int)(mylon*100));
			fputs(mychar,Traces);
			
			append=".";
			app=append.c_str();
			fputs(app,Traces);
			
			mychar = doubletostr(mylon*100-(int)(mylon*100),6);
			fputs(mychar,Traces);
			// $ GPRMC,201512,A,1234.23456,N,1234.1234,
			append=",E,,,,0.0,,,,,\n";
			app=append.c_str();
			fputs(app,Traces);
			
			lauf++;
		}
	
	// $GPGGA,163156,5152.681389,N,00745.598541,E,,,,0.0,,,,,

		
	fclose(Traces);
}

Graph::MergeVertex()
{
	/*Toleranzwert für den Abstand zweier zu verschmelzender Knoten*/
	int tolerance=10;
	/*Für die Länge einer Kante*/
	int length;
	/*Hilfszeiger*/
	Edge* aktuell;
	/*Für den Start- und Endknoten einer Kante*/
	Vertex* start;
	Vertex* end;

	/*Nacheinander holen jeder Kante*/
	for(unsigned i=0; i<eArray.size(); i++)
	{
		aktuell=eArray[i];
		length=aktuell->GetLength();
		/*Überprüfung ob Start- und Endknoten zu Nahe beieinander liegen*/
		if(length<tolerance)
		{
			start=aktuell->GetStartingVertex();
			end=aktuell->GetEndingVertex();
			
			/*Koordinaten der aktuellen Start- und Endknoten*/
			int x1 = start->GetX();
			int y1 = end  ->GetY();
			int x2 = start->GetX();
			int y2 = end  ->GetY();
			/*Koordinaten des neuen Knotens*/
			int(xneu)=(x1+x2)/2;
			int(yneu)=(y1+y2)/2;
		
			/*Löschen der zu kurzen Kanten*/
			DelEdge(start,end);
			DelEdge(end,start);

			/*Hinzufügen des neuen Kotens*/
			AddVertexByXY(xneu,yneu);
			/*Zeiger auf den neuen Knoten*/
			Vertex* neu = SearchVertex(xneu,yneu);

			
			/*Hilfsschleife (von Miguel rausgenommen)*/
			/*int v = eArray.size();
			for(unsigned int go = 0 ; go<v ;go++)
				
			{
			*/	
				/*Hole jede Kante*/
				for(unsigned int j=0; j<eArray.size(); j++)
				{
					Edge* aktuell2 = eArray[j];
					Vertex* start1 = aktuell2->GetStartingVertex();
					Vertex* end1   = aktuell2->GetEndingVertex();
			
					
					/*Es folgt eine Überprüfung, ob von der aktuell2 Kante der Start oder Endknoten
					mit einem Knoten der aktuell-Kante übereinstimmt
					So bekommt man die nachfolger von Start und EndKnoten*/

					if(start1==start || end1==start)
					{
						DelEdge(start1,end1);
						DelEdge(end1,start1);
						if(start1==start)
						{
						mArray.push_back(end1);
						}
						
						else mArray.push_back(start1);
						j--;
						if (j>=0) j--;
					}

					if(start1==end   || end1==end)
					{
						DelEdge(start1,end1);
						DelEdge(end1,start1);
						if(start1==end)
						{
							mArray.push_back(end1);
						}
						else mArray.push_back(start1);
						j--;
						if (j>=0) j--;
					}
				}
			//}

			/*Löschen der Start- und Endknoten der zu kurzen Kante*/
			DelVertex4Merge(start);
			DelVertex4Merge(end);

			
			/*Hinzufügen von neuen Kanten von den Nachfolgern von Start- und Endknoten zum*/
			/*jeweiligen neuen Knoten*/

			for(unsigned int m_Node=0; m_Node<mArray.size();m_Node++)
			{
				
				Vertex* myNode = mArray[m_Node];
				AddEdgeByVertex(myNode,neu);
				AddEdgeByVertex(neu,myNode);
			}

			mArray.clear();
			
			//neu von miguel
			
			/*
				Da vorne im EdgeArray
			    Kanten gelöscht werden, 
			    muß die Laufvariable 
			    zurückgesetzt werden 
			*/ 
			
			i--;
			if (i>=0) i--;
		}
	}
}

/*Löschmethode für einen Knoten für den Merger*/
Graph::DelVertex4Merge(Vertex* v)
{
	int i= GetIndex(v);
	if(i!=1000)
	{
		vArray.erase(vArray.begin()+i);
		DelNachfolger4Merge(v);
	}
}

/*Löscht alle Nachfolgenden Kanten eines Knotens*/
Graph::DelNachfolger4Merge(Vertex* v)
{
	Vertex* temp;
	for(int i=0; i<v->GetNachfolger(); i++)
	{
		temp=v->GetNachfolgeVertex(i);
		DelEdge(v,temp);
		DelEdge(temp,v);
	}
}


