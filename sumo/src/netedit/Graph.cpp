// Graph.cpp: Implementierung der Klasse Graph.
//
//////////////////////////////////////////////////////////////////////

#include "Graph.h"

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

    int anzahl=vArray.size();

    for (m;m<anzahl;m++)
    {
        n=0;
        while (n<vArray.size())
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
                    break;
                }
            }
            n++;
        }
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
    int Anzahl = eArray.size();
    for(int j=0; j<Anzahl; j++)
    {
        for(i=0; i<eArray.size();i++)
        {
            ptemp=eArray[i];
            if((ptemp->GetStartingVertex()==v)&&(ptemp->GetEndingVertex()==w))
            {
                eArray.erase(eArray.begin()+i);
                v->DekrementOutDegree();
                w->DekrementInDegree();
            }
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

