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

//            ptemp->DelInzidentEdge();
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
Graph::Drive(int i)
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
    for(long i=0; i<eArray.size();i++)
    {
        etemp=eArray[i];
        if(etemp->GetLength()==0)
        {
            DelDoubleEdge(etemp->GetStartingVertex(),etemp->GetEndingVertex());
            i--;
			break;
        }
    }
    
}
/////////////////////////ENDE NEU!!!!!!!!!!!!!!////////////////////

void
Graph::Export_Vertexes_XML()
{
    string append;
    string append2;
    append="<nodes>\n";
    append2="</nodes>\n";
    const char* app=append.c_str();
    const char* app2=append2.c_str();
    char buffer [100];
	int Vertices= vArray.size();
	Vertex* temp;
    
	FILE* vfile= fopen("export.nod.xml","w");
    
	fputs(app,vfile);
    for(long i=0 ; i<Vertices; i++)
    {
        temp=vArray[i];
		int x=temp->GetX();
        int y=temp->GetY();
		sprintf (buffer, "<node id=\"%d\" x=\"%d.0\" y=\"%d.0\" type=\"priority\"/>\n",i,x,y);
		fputs(buffer, vfile);
	}
   	fputs(app2,vfile);
	fclose(vfile);
}

void
Graph::Export_Edges_XML()
{
    string append;
    string append2;
    append="<edges>\n";
    append2="</edges>\n";
    const char* app=append.c_str();
    const char* app2=append2.c_str();
    Edge* temp;
    Vertex* start;
    Vertex* end;
    int index_start=0;
    int index_end=0;
	char buffer [100];

	FILE* efile= fopen("export.edg.xml","w");
    
    fputs(app,efile);

    for(long i=0; i<eArray.size(); i++)
    {
        temp=eArray[i];
        start=temp->GetStartingVertex();
		end=temp->GetEndingVertex();
		index_start=GetIndex(start);
		index_end=GetIndex(end);
		if ((index_start!=-1)&&(index_end!=-1))
		{
			sprintf (buffer, "<edge id=\"%d\" fromnode=\"%d\" tonode=\"%d\" priority=\"78\" nolanes=\"1\" speed=\"15.000\"/>\n",i,index_start,index_end);
			fputs(buffer,efile);
		}
	}
    fputs(app2,efile);
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
	char buffer1 [100];
	char buffer2 [100];
	char buffer3 [100];
	double mylat;
	double mylon;
	int mylat1;
	int mylon1;
	long mylat2;
	long mylon2;
	time_t rawtime;
	tm* ptm;
	Vertex* ptemp;

	for (int l=1; l<=cars; l++)
	{
		sprintf(buffer1,"Trace%2d.txt",l);
		FILE* Traces = fopen(buffer1,"w");
		Drive(fuel);
		pfad = GetPfadArray();
		for (int m=0; m<pfad.size(); m++)
		{
			ptemp=pfad[m];
			time(&rawtime);
			ptm=gmtime(&rawtime);
			mylat=ptemp->GetGPSLat()*100;
			mylon=ptemp->GetGPSLon()*100;
			mylat1=(int)mylat;
			mylon1=(int)mylon;
			mylat2=(long)((mylat-(int)mylat)*1000000);
			mylon2=(long)((mylon-(int)mylon)*1000000);
			//So soll es aussehen
			//$GPRMC,163156,A,5152.681389,N,00745.598541,E,,,26102004,,
			//$GPGGA,163156,5152.681389,N,00745.598541,E,,,,0.0,,,,,

			sprintf(buffer2,"$GPRMC,%02d%02d%02d,A,%04d.%06d,N,%05d.%06d,E,,,%02d%02d%d,,\n",ptm->tm_hour+1,ptm->tm_min,ptm->tm_sec,mylat1,mylat2,mylon1,mylon2,ptm->tm_mday,ptm->tm_mon+1,ptm->tm_year+1900);
			fputs(buffer2,Traces);
			sprintf(buffer3,"$GPGGA,%02d%02d%02d,%04d.%06d,N,%05d.%06d,E,,,,0.0,,,,,\n",ptm->tm_hour+1,ptm->tm_min,ptm->tm_sec,mylat1,mylat2,mylon1,mylon2);
			fputs(buffer3,Traces);
		}
		fclose(Traces);
		
	}
}

Graph::MergeVertex()
{
	/*Toleranzwert für den Abstand zweier zu verschmelzender Knoten*/
	int tolerance=10;
	/*Für die Länge einer Kante*/
	int length;
	/*Hilfszeiger*/
	Edge* aktuell;
	Edge* aktuell2;
	/*Für den Start- und Endknoten einer Kante*/
	Vertex* start;
	Vertex* end;
	Vertex* start2;
	Vertex* end2;
		
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
			
			//Koordinaten der aktuellen Start- und Endknoten
		    //werden gemittelt zu den Koordinaten des neuen Knotens
			int(xneu)=(start->GetX()+end->GetX())/2;
			int(yneu)=(start->GetY()+end->GetY())/2;
		
			/*Löschen der zu kurzen Kanten*/
			DelEdge(start,end);
			DelEdge(end,start);

			/*Hinzufügen des neuen Kotens*/
			AddVertexByXY(xneu,yneu);
			/*Zeiger auf den neu hinzugefügten Knoten*/
			Vertex* neu = SearchVertex(xneu,yneu);
			
			/*Hole jede Kante*/
			for(unsigned int j=0; j<eArray.size(); j++)
			{
				aktuell2 = eArray[j];
				start2 = aktuell2->GetStartingVertex();
				end2   = aktuell2->GetEndingVertex();
				
				/*Es folgt eine Überprüfung, ob von der aktuell2 Kante der Start oder Endknoten
				mit einem Knoten der aktuell-Kante übereinstimmt
				So bekommt man die nachfolger von Start und EndKnoten*/

				if(((start2==start)||(end2==start))||((start2==end)||(end2==end)))
				{
					DelEdge(start2,end2);
					DelEdge(end2,start2);
					if((start2==start)||(start2==end)) mArray.push_back(end2);
					else mArray.push_back(start2);
					j-=2;
					if (j<-1) j=-1;
				}
			}
			
			/*Löschen der Start- und Endknoten der zu kurzen Kante*/
			DelVertex4Merge(start);
			DelVertex4Merge(end);
			/*Hinzufügen von neuen Kanten von den Nachfolgern von Start- und Endknoten zum*/
			/*jeweiligen neuen Knoten*/
			for(unsigned int m_Node=0; m_Node<mArray.size();m_Node++)
			{
				AddEdgeByVertex(mArray[m_Node],neu);
				AddEdgeByVertex(neu,mArray[m_Node]);
			}
			mArray.clear();
			//Da vorne im EdgeArray Kanten gelöscht werden, muß die Laufvariable zurückgesetzt werden
			i-=2;
			if (i<-1) i=-1;
		}
	}
}

/*Löschmethode für einen Knoten für den Merger*/
Graph::DelVertex4Merge(Vertex* v)
{
	if(GetIndex(v)!=-1)
	{
		vArray.erase(vArray.begin()+GetIndex(v));
		DelNachfolger4Merge(v);
	}
}

/*Löscht alle Nachfolgenden Kanten eines Knotens*/
Graph::DelNachfolger4Merge(Vertex* v)
{
	for(int i=0; i<v->GetNachfolger(); i++)
	{
		DelEdge(v,v->GetNachfolgeVertex(i));
		DelEdge(v->GetNachfolgeVertex(i),v);
	}
}


