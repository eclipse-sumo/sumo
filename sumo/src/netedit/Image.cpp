// Image.cpp: Implementierung der Klasse Image.
//
//////////////////////////////////////////////////////////////////////

// Diese Klasse stellt die Grundlage der Bildbearbeitung dar

#include "Image.h"
#include <fx.h>

Image::Image(FXImage *img,FXApp *a)
{
    m_img=img;
    m_transimg =
        new FXImage(a,new unsigned int[4],IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP, 2, 2);
    m_transimg->scale(img->getWidth(),img->getHeight());
}

//Kopiere ein Image in ein anderes Image
void
Image::Copy(FXImage *source,FXImage *destination)
{
    //Durchlaufe das Image
    for (int i=0 ; i<source->getWidth() ; ++i)
        for (int j=0; j<source->getHeight() ; ++j)
        {
            //Kopiere aktuelles Pixel
            destination->setPixel(i,j,source->getPixel(i,j));
        }

}

//Hole das Anzeigeimage
FXImage* Image::GetFXImage()
{
    return m_img;
}

/*
Erzeugt ein leeres Image
*/
void
Image::EmptyImage()
{
    //Hole Höhe und Breite des Image
    FXint wid = m_img->getWidth();
    FXint hei = m_img->getHeight();

    //Durchlaufe das Image
    for (FXint i=0 ; i<wid ; ++i)
        for (FXint j=0; j<hei ; ++j)
            //Setze die Farbe des aktuellen Pixels auf weiß
            m_img->setPixel(i,j,FXRGB(255,255,255));
}
/*
Erzeugt aus dem Ausgangsbild ein Schwarzweißbild,
in denen die für uns relevanten Farben in schwarz
und alle anderen Farben in weiß gefärbt werden
*/
void Image::ExtractStreets()
{
    //Hole Höhe und Breite des Image
    FXint wid = m_img->getWidth();
    FXint hei = m_img->getHeight();

    //Durchlaufe das Image
    for (FXint i=0 ; i<wid ; ++i)
        for (FXint j=0; j<hei ; ++j)
        {
            //Hole die Farbe des aktuellen Pixels
            FXColor col=m_img->getPixel(i,j);

            //Ist das Pixel in einer der gewünschen Farben, setze es auf schwarz, sonst auf weiß
            if(
                //weiß
                (col==FXRGB(255,255,255))||
                //gelb
                (col==FXRGB(255,255,0))||
                //schwarz
                (col==FXRGB(0,0,0))
                )
                //Setzen auf schwarz
                m_img->setPixel(i,j,FXRGB(0,0,0));

            else
                //Setzen auf weiß
                m_img->setPixel(i,j,FXRGB(255,255,255));
        }
}

/*
Führt eine Erosion durch, d.h. eine Verdünnung
der Schwärzen Flächen des Bildes
*/
void Image::Erode()
{
    //Hole Höhe und Breite des Image
    FXint wid = m_img->getWidth();
    FXint hei = m_img->getHeight();

    //Integer-Varaiblen für die Schleifendurchläufe
    FXint i,j,k,m;

    bool tester= true;

    //Durchlaufe das Image
    for (i=0 ; i<wid ; i++)
        for (j=0; j<hei ; j++)
        {
             //Überprüfe die Nachbarn des aktuellen Pixels
            for (k=i-1 ; k<=i+1 ; k++)
                for (m=j-1 ; m<=j+1 ;m++)
                {
                    //Vermeide, daß auf Pixel außerhalb des Image zugegriffen wird
                    if ((k>0)&&(k<wid)&&(m>0)&&(m<hei))
                    {
                        //Hole die Farbe des aktuellen Pixels
                        FXColor col=m_img->getPixel(k,m);

                        //Wenn Pixel nicht schwarz ist, setze tester auf false
                        if(!(col==FXRGB(0,0,0)))
                            tester = false;
                    }
                }

        // Wenn wir keine nichtschwarzen Nachbarpixel haben, bleibt das Pixel schwarz
        if (tester == true)
            m_transimg->setPixel(i,j,FXRGB(0,0,0));

        // Sonst wird es auf weiß gesetzt
        else
            m_transimg->setPixel(i,j,FXRGB(255,255,255));//weiß
        tester=true;
        }

    //Kopiere das Bearbeitungsimage in das Anzeigeimage
    Copy(m_transimg,m_img);
}

/*
Invers zur Erosion, d.h. hier werden die schwarzen
Flächen verbreitert
*/
void Image::Dilate()
{
    //Siehe Methode Erode
    int wid = m_img->getWidth();
    int hei = m_img->getHeight();
    int i,j,k,m;
    bool tester = true;
    for (i=0 ; i<wid ; ++i)
        for (j=0; j<hei ; ++j)
        {
            //Überprüfe die Nachbarn des aktuellen Pixels
            for (k=i-1 ; k<=i+1 ; ++k)
                for (m=j-1 ; m<=j+1 ;++m)
                {
                    //Vermeide, daß auf Pixel außerhalb des Image zugegriffen wird
                    if ((k>0)&&(k<wid)&&(m>0)&&(m<hei))
                    {
                        FXColor col=m_img->getPixel(k,m);
                        //Wenn Farbe nicht weiß ist, setze tester auf false
                        if(col!=FXRGB(255,255,255))
                            tester = false;

                    }
                }

        // Wenn wir keine nichtweißen Nachbarpixel haben, bleibt das Pixel weiß
        if (tester == true)
            m_transimg->setPixel(i,j,FXRGB(255,255,255));

        // Sonst wird es auf schwarz gesetzt
        else
            m_transimg->setPixel(i,j,FXRGB(0,0,0));
        tester = true;
        }
    Copy(m_transimg,m_img);
}

/*
Führt erst eine Erosion, dann eine Dilatation durch
*/
void Image::Opening()
{
    Erode();
    Dilate();
}

/*
Invers zu Opening(Dilatation->Erosion)
*/
void Image::Closing()
{
    Dilate();
    Erode();
}

void Image::CloseGaps()
{
    //Kopiere Anzeigeimage in Bearbeitungsimage
    Copy(m_img,m_transimg);

    //Hole Höhe und Breite des Image
    int wid=m_img->getWidth();
    int hei=m_img->getHeight();

    //Zähler für die nichtweißen Pixel um das aktuelle Pixel
    int counter=0;

    //Integer-Variablen für Schleifendurchläufe
    int i,j,k,m;

    //Durchlaufe das Image
    for (i=0 ; i<wid ; i++)
        for (j=0; j<hei ; j++)
        {
            //Überprüfe die Nachbarn des aktuellen Pixels
            for (k=i-1 ; k<=i+1 ; k++)
                for (m=j-1 ; m<=j+1 ;m++)
                {
                    //Vermeide, daß auf Pixel außerhalb des Image zugegriffen wird
                    if ((k>0)&&(k<wid)&&(m>0)&&(m<hei))
                    {
                        //Zähle nichtweiße Pixel um aktuelles Pixel
                        if (m_img->getPixel(k,m)!=FXRGB(255,255,255))
                            counter = counter + 1;
                    }
                }
        //Wenn das aktuelle Pixel weiß ist und es mindestens fünf schwarze Nachbarpixel hat,
        //dann setze das aktuelle Pixel auf schwarz
        if ((m_img->getPixel(i,j)==FXRGB(255,255,255))&&counter >= 5)
            m_transimg->setPixel(i,j,FXRGB(0,0,0));

        //Counter für nächsten Schleifendurchlauf zurücksetzen
        counter =0;
        }
    //Setze das Anzeigeimage auf das veränderte Image
    Copy(m_transimg,m_img);
}

void Image::CreateSkeleton()
{
    //Hole die Breite und Höhe des Image
    int wid =m_img->getWidth();
    int hei =m_img->getHeight();

    //Anzahl der schwarzgefärbten Nachbarpixel
    int black_neighbours=0;

    bool any_changes=true;
    bool white_neighbour = false;

    //Erstelle eine Kopie des Image m_img mit Namen m_transimg
    Copy(m_img,m_transimg);
    // Verdünne das Bild bis keine Veränderungen mehr stattfinden
    while (any_changes==true)
    {
    any_changes=false;
        //Betrachte alle Pixel des Bildes ohne den Rand
        for(int i=1 ; i<wid-1 ; ++i)
        {
            for(int j=1 ; j<hei-1 ; ++j)
            {
                //Betrachte das aktuelle Pixel nur weiter, falls es schwarz ist
                if (m_img->getPixel(i,j)==FXRGB(0,0,0))
                {
                    //Untersuche, ob das aktuelle (schwarze) Pixel einen weißen Nachbarn hat.. dann ist es ein(Strassen)-Randpixel
                    for (int k=i-1 ; k<=i+1 ; ++k)
                        for (int l=j-1 ; l<=j+1 ; ++l)
                            if(m_img->getPixel(k,l)==FXRGB(255,255,255))
                                white_neighbour = true;
                    if (white_neighbour == true)
                    {
                        //Untersuche, ob die Anzahl der schwarzen Nachbarn des aktuellen Pixels zwischen 2 und 6 liegt
                        //Beachte, daß der aktuelle (schwarze) Pixel mitgezählt wird.. deswegen: if ((black_neighbours>=3)&&(black_neighbours<=7))
                        for (int k=i-1 ; k<=i+1 ; ++k)
                            for (int l=j-1 ; l<=j+1 ; ++l)
                                if(m_img->getPixel(k,l)==FXRGB(0,0,0))
                                    ++black_neighbours;
                        if ((black_neighbours>=3)&&(black_neighbours<=7))
                        {

                            //Zähle die Anzahl der weiß-schwarz Übergänge (siehe auch Methode CountTransitions)
                            if(CountTransitions(i,j)==1)
                            {
                                if((
                                    (m_img->getPixel(i+1,j)==FXRGB(255,255,255))||
                                    (m_img->getPixel(i,j+1)==FXRGB(255,255,255))
                                  )
                                  ||
                                  (
                                    (m_img->getPixel(i,j-1)==FXRGB(255,255,255))&&
                                    m_img->getPixel(i-1,j)==FXRGB(255,255,255))
                                  )
                                {
                                    m_transimg->setPixel(i,j,FXRGB(255,255,255));
                                    any_changes=true;
                                }
                            }
                        }
                    }
                }
            //Setze die Prüfvariablen zurück
            black_neighbours=0;
            white_neighbour=false;

            }
        }
        //Kopiere das Bearbeitungsimage in das Anzeigeimage
        Copy(m_transimg,m_img);
        //Betrachte alle Pixel des Bildes außer dem Rand
        for(i=1 ; i<wid-1 ; ++i)
        {
            for(int j=1 ; j<hei-1 ; ++j)
            {
                //Falls das aktuelle Pixel schwarz ist
                if (m_img->getPixel(i,j)==FXRGB(0,0,0))
                {
                    //Untersuche, ob das aktuelle (schwarze) Pixel einen weißen Nachbarn hat.. dann ist es ein(Strassen)-randpixel
                    for (int k=i-1 ; k<=i+1 ; ++k)
                        for (int l=j-1 ; l<=j+1 ; ++l)
                            if(m_img->getPixel(k,l)==FXRGB(255,255,255))
                                white_neighbour = true;
                    if (white_neighbour == true)
                    {
                        //Untersuche, ob die Anzahl der schwarzen Nachbarn des aktuellen Pixels zwischen 2 und 6 liegt
                        //Beachte, daß der aktuelle (schwarze) Pixel mitgezählt wird.. deswegen: if ((black_neighbours>=3)&&(black_neighbours<=7))
                        for (int k=i-1 ; k<=i+1 ; ++k)
                            for (int l=j-1 ; l<=j+1 ; ++l)
                                if(m_img->getPixel(k,l)==FXRGB(0,0,0))
                                    ++black_neighbours;
                        if ((black_neighbours>=3)&&(black_neighbours<=7))
                        {
                            //Zähle die Anzahl der weiß-schwarz Übergänge (siehe auch Methode CountTransitions)
                            if(CountTransitions(i,j)==1)
                            {
                                if((
                                    (m_img->getPixel(i,j-1)==FXRGB(255,255,255))||
                                    (m_img->getPixel(i-1,j)==FXRGB(255,255,255))
                                  )
                                  ||
                                  (
                                    (m_img->getPixel(i+1,j)==FXRGB(255,255,255))&&
                                    m_img->getPixel(i,j+1)==FXRGB(255,255,255))
                                  )
                                {
                                    m_transimg->setPixel(i,j,FXRGB(255,255,255));
                                    any_changes=true;
                                }
                            }
                        }
                    }
                }
            //Setze die Prüfvariablen zurück
            black_neighbours=0;
            white_neighbour=false;

            }
        }
    //Kopiere das veränderte Image zurück in das Anzeigeimage
    Copy(m_transimg,m_img);
    }
	RarifySkeleton();
}

void Image::RarifySkeleton()
{
    //Hole die Breite und Höhe des Image
    int wid = m_img->getWidth();
    int hei = m_img->getHeight();

    //Anzahl der schwarzgefärbten Nachbarpixel
    int black_neighbours =0;

    //Betrachte alle Pixel des Bildes außer dem Rand
    for(int i=1 ; i<wid-1 ; ++i)
        for(int j=1 ; j<hei-1 ; ++j)
            {
                //Betrachte das aktuelle Pixel nur weiter, falls es schwarz ist
                if (m_img->getPixel(i,j)==FXRGB(0,0,0))
                {
                    //Zähle die schwarzen Nachbarn des aktuellen Pixels
                    for (int k=i-1 ; k<=i+1 ; ++k)
                        for (int l=j-1 ; l<=j+1 ; ++l)
                            if(m_img->getPixel(k,l)==FXRGB(0,0,0))
                                ++black_neighbours;
                    if (black_neighbours==3)
                    {
                        if(
                                (
                                    (m_img->getPixel(i,j-1)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i+1,j)==FXRGB(0,0,0))
                                )
                                ||
                                (
                                    (m_img->getPixel(i+1,j)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i,j+1)==FXRGB(0,0,0))
                                )
                                ||
                                (
                                    (m_img->getPixel(i,j+1)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i-1,j)==FXRGB(0,0,0))
                                )
                                ||
                                (
                                    (m_img->getPixel(i-1,j)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i,j-1)==FXRGB(0,0,0))
                                )
                            )
                        {
                          //Setze Pixel (i,j) auf weiß
                          m_img->setPixel(i,j,FXRGB(255,255,255));
                        }

                    }
                    if (black_neighbours==4)
                    {

                        if
                        (
                            (
                            (
                                (
                                    (m_img->getPixel(i,j-1)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i+1,j)==FXRGB(0,0,0))
                                )
                                ||
                                (
                                    (m_img->getPixel(i+1,j)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i,j+1)==FXRGB(0,0,0))
                                )
                                ||
                                (
                                    (m_img->getPixel(i,j+1)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i-1,j)==FXRGB(0,0,0))
                                )
                                ||
                                (
                                    (m_img->getPixel(i-1,j)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i,j-1)==FXRGB(0,0,0))
                                )
                            )&&
                                //Zähle die Anzahl der weiß-schwarz Übergänge (siehe auch Methode CountTransitions)
                                (CountTransitions(i,j)<=2)
                            )
                            ||
                            (
                                (
                                    (m_img->getPixel(i,j-1)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i+1,j)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i,j+1)==FXRGB(0,0,0))
                                )
                                ||
                                (
                                    (m_img->getPixel(i+1,j)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i,j+1)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i-1,j)==FXRGB(0,0,0))
                                )
                                ||
                                (
                                    (m_img->getPixel(i,j+1)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i-1,j)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i,j-1)==FXRGB(0,0,0))
                                )
                                ||
                                (
                                    (m_img->getPixel(i-1,j)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i,j-1)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i+1,j)==FXRGB(0,0,0))
                                )
                            )
                        )
                        {
                            //Setze Pixel (i,j) auf weiß
                            m_img->setPixel(i,j,FXRGB(255,255,255));
                        }

                    }
                    if(black_neighbours==5)
                    {

                        if
                        (
						(
                            !(
                                (
                                    (m_img->getPixel(i,j-1)==FXRGB(255,255,255))&&
                                    (m_img->getPixel(i,j+1)==FXRGB(255,255,255))
                                )
                                ||
                                (
                                    (m_img->getPixel(i-1,j)==FXRGB(255,255,255))&&
                                    (m_img->getPixel(i+1,j)==FXRGB(255,255,255))
                                )
                            )
                            &&
                            //Zähle die Anzahl der weiß-schwarz Übergänge (siehe auch Methode CountTransitions)
                            (CountTransitions(i,j)<=2)
							||
							(
								(m_img->getPixel(i,j-1)==FXRGB(0,0,0))&&
								(m_img->getPixel(i+1,j)==FXRGB(0,0,0))&&
								(m_img->getPixel(i,j-1)==FXRGB(0,0,0))&&
								(m_img->getPixel(i-1,j)==FXRGB(0,0,0))
							)
							||
							(
								(
                                    (m_img->getPixel(i,j-1)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i+1,j)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i,j+1)==FXRGB(0,0,0))
                                )
                                ||
                                (
                                    (m_img->getPixel(i+1,j)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i,j+1)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i-1,j)==FXRGB(0,0,0))
                                )
                                ||
                                (
                                    (m_img->getPixel(i,j+1)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i-1,j)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i,j-1)==FXRGB(0,0,0))
                                )
                                ||
                                (
                                    (m_img->getPixel(i-1,j)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i,j-1)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i+1,j)==FXRGB(0,0,0))
                                )
							)
							)
							&&
							!(
								(
									(m_img->getPixel(i-1,j)==FXRGB(0,0,0))&&
									(m_img->getPixel(i-1,j-1)==FXRGB(0,0,0))&&
									(m_img->getPixel(i,j-1)==FXRGB(0,0,0))&&
									(m_img->getPixel(i+1,j+1)==FXRGB(0,0,0))
								)
								||
								(
									(m_img->getPixel(i,j-1)==FXRGB(0,0,0))&&
									(m_img->getPixel(i+1,j-1)==FXRGB(0,0,0))&&
									(m_img->getPixel(i+1,j)==FXRGB(0,0,0))&&
									(m_img->getPixel(i-1,j+1)==FXRGB(0,0,0))
								)
								||
								(
									(m_img->getPixel(i+1,j)==FXRGB(0,0,0))&&
									(m_img->getPixel(i+1,j+1)==FXRGB(0,0,0))&&
									(m_img->getPixel(i,j+1)==FXRGB(0,0,0))&&
									(m_img->getPixel(i-1,j-1)==FXRGB(0,0,0))
								)
								||
								(
									(m_img->getPixel(i,j+1)==FXRGB(0,0,0))&&
									(m_img->getPixel(i-1,j+1)==FXRGB(0,0,0))&&
									(m_img->getPixel(i-1,j)==FXRGB(0,0,0))&&
									(m_img->getPixel(i+1,j-1)==FXRGB(0,0,0))
								)
							)
                        )
                            //Setze Pixel (i,j) auf weiß
                            m_img->setPixel(i,j,FXRGB(255,255,255));
                        if(
                            (CountTransitions(i,j)==3)
                            &&
                            (
                                (
                                    (m_img->getPixel(i,j-1)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i,j+1)==FXRGB(0,0,0))
                                )
                                ||
                                (
                                    (m_img->getPixel(i-1,j)==FXRGB(0,0,0))&&
                                    (m_img->getPixel(i+1,j)==FXRGB(0,0,0))
                                )
                            )
                          )
                            //Setze Pixel (i,j) auf weiß
                            m_img->setPixel(i,j,FXRGB(255,255,255));
                    }
                }

                black_neighbours=0;
            }
}


Graph Image::Tracking(Graph gr)
{
    Vertex* temp=NULL;

    //Hole die Breite und Höhe des Image
    int wid = m_img->getWidth();
    int hei = m_img->getHeight();

    for(int g=0 ; g<wid ;++g)
    {
        m_img->setPixel(g,0,FXRGB(255,255,255));
        m_img->setPixel(g,hei-1,FXRGB(255,255,255));
    }
    for(int h=0 ; h<hei ;++h)
    {
        m_img->setPixel(0,h,FXRGB(255,255,255));
        m_img->setPixel(wid-1,h,FXRGB(255,255,255));
    }

    //Durchlaufe das Bild außer dem Rand
    for (int i=1 ; i<wid-1 ; ++i)
        for (int j=1; j<hei-1 ; ++j)
            //Falls der aktuelle Pixel schwarz ist, wird Pixel_Counter aufgerufen.
            //Pixel_Counter erzeugt dann den Graphen, für die an diesem schwarzen Pixel hängende Zusammenhangskomponente
            //dabei muß darauf geachtet werden, daß die ´besuchten´ Pixel nicht nocheinmal besucht werden
            if(m_img->getPixel(i,j)==FXRGB(0,0,0))
            {
                //´-1,-1´ bedeutet, daß kein Vorgängerpixel existiert
                Graph hilfsgr=Pixel_Counter(i,j,-1,-1,0,gr,temp);

                //Nur wenn die neue Zusammenhangskomponente mehr als 20 Knoten enthält, wird sie in den Graphen aufgenommen
                if (hilfsgr.Number_of_Vertex()-gr.Number_of_Vertex()>20)
                    gr=hilfsgr;
            }
	gr.Reduce_plus();
	gr.MergeVertex();
    return gr;
}


Graph Image::Pixel_Counter(int i,int j,int i_pre, int j_pre,int count,Graph gr, Vertex* temp)
{

    int neighbour_counter;
    int p_counter=count;
    bool changed=false;
    bool closed_circle=false;
    list<Point> n_List;
    list<Point>::iterator iter;
    n_List.clear();
    
	/*Solange der aktuelle Pixel
	  genau einen Nachbarn hat
	  (ohne Vorgänger), wird die
	  Schleife ausgeführt
    */
	do
    {
        n_List.clear();
        neighbour_counter=0;
        //Zähle die schwarzen Nachbarpixel des aktuellen Pixels
        //Zähle dabei aber nicht den Vorgängerpixel mit...(k!=i_pre)&&(l!=j_pre)
        
		for (int k=i-1 ; k<=i+1 ; ++k)
            for (int l=j-1 ; l<=j+1 ; ++l)
                if      (
                        (m_img->getPixel(k,l)==FXRGB(0,0,0)&&!((k==i)&&(l==j))&&!((k==i_pre)&&(l==j_pre)))
                        ||
                        (m_img->getPixel(k,l)==FXRGB(0,255,0)&&!((k==i)&&(l==j))&&!((k==i_pre)&&(l==j_pre)))
                        )
                    {
                        ++neighbour_counter;
                        Point *pt=new Point(k,l);
                        n_List.push_back(*pt);
                    }
        
		////////////////// 0 Sekunden ///////////////////////////
		//Der Pixel ist ein Endpunkt.. erzeuge Knoten
        if(neighbour_counter==0)
        {
            for (int k=i-1 ; k<=i+1 ; ++k)
                for (int l=j-1 ; l<=j+1 ; ++l)
                    if(m_img->getPixel(k,l)==FXRGB(0,0,255))
                    {
                        m_img->setPixel(k,l,FXRGB(255,0,0));
                        m_img->setPixel(i,j,FXRGB(255,0,0));
                        closed_circle=true;
                        for (int n=k-1 ; n<=k+1 ; ++n)
						    for (int m=l-1 ; m<=l+1 ; ++m)
                                if(m_img->getPixel(n,m)==FXRGB(255,255,0))
                                    {
                                        Vertex* temp2=gr.SearchVertex(n,m);
                                        gr.AddEdgeByVertex(temp,temp2);
                                        gr.AddEdgeByVertex(temp2,temp);
                                    }
                    }
                    if(closed_circle==false)
                    {
                        //gelb.. Knoten wird erzeugt
                        m_img->setPixel(i,j,FXRGB(255,255,0));
                        gr.AddVertexByXY(i,j);
                        if(temp!=NULL)
                        {
                            gr.AddEdgeByVertex(temp,gr.SearchVertex(i,j));
                            gr.AddEdgeByVertex(gr.SearchVertex(i,j),temp);
                        }
                        temp=gr.SearchVertex(i,j);
                    }
        }
		////////////////////// 0 Sekunden Ende ////////////////////////////
		
        //Der Pixel liegt auf einer ´Linie´..
        if(neighbour_counter==1)
        {
            if((p_counter>0)&&(p_counter<20))
            {
                ++p_counter;
                //rot .. der Pixel wird gelöscht
                m_img->setPixel(i,j,FXRGB(255,0,0));
            }
            else
            {
                if(p_counter==20)
                    p_counter=0;
                ++p_counter;
                //gelb
                m_img->setPixel(i,j,FXRGB(255,255,0));
                gr.AddVertexByXY(i,j);
                if(temp!=NULL)
				{
                    gr.AddEdgeByVertex(temp,gr.SearchVertex(i,j));
                    gr.AddEdgeByVertex(gr.SearchVertex(i,j),temp);
                }
                temp=gr.SearchVertex(i,j);
            }
        }
        
		if(n_List.size()==1)
        {
            i_pre=i;
            j_pre=j;
            i=n_List.front().GetX();
            j=n_List.front().GetY();
        }
        else
        {
		    iter=n_List.begin();
			while (iter !=n_List.end())
			{
				m_img->setPixel(iter->GetX(),iter->GetY(),FXRGB(0,0,255));
				iter++;
			}
        }
        changed=true;
	}
    while(neighbour_counter==1);
	
    if(!(n_List.empty()))
    {
        //gelb
        m_img->setPixel(i,j,FXRGB(255,255,0));
        gr.AddVertexByXY(i,j);
        if(temp!=NULL)
		{
            gr.AddEdgeByVertex(temp,gr.SearchVertex(i,j));
            gr.AddEdgeByVertex(gr.SearchVertex(i,j),temp);
        }
        temp=gr.SearchVertex(i,j);
        iter=n_List.begin();
        while (iter !=n_List.end())
        {
            if(m_img->getPixel(iter->GetX(),iter->GetY())==FXRGB(0,0,255))
				gr=Pixel_Counter(iter->GetX(),iter->GetY(),i,j,1,gr,temp);
            iter++;
        }
    }
    n_List.clear();
	
    return gr;
}


void Image::EraseStains(int deep)
{
    //Hole Breite und Höhe des Image
    int wid = m_img->getWidth();
    int hei = m_img->getHeight();

    //Durchlaufe das Image
    for(int i =0+deep; i<wid-deep ; ++i)
    {
        for (int j =0+deep; j<hei-deep ; ++j)
        {
            //schwarzer Pixel gefunden
            if(m_img->getPixel(i,j)==FXRGB(0,0,0))
            {
                //Eckpunkte des Quadrates definieren entsprechend der gewählten Tiefe
                int i_links = i-((deep-1)/2);
                int i_rechts= i+((deep-1)/2);
                int b_oben  = j-1;
                int b_unten = j+(((deep-1)/2)-1);
                int black = 0;

                // Durchlaufe alle vier Seiten des Quadrates und suche schwarze Pixel

                // von oben links nach unten links
                for(int d =b_oben+1 ; d<=b_unten-1 ; ++d)
                {
                    if(m_img->getPixel(i_links,d)==FXRGB(0,0,0)) black = black+1;
                    if(black>0)break;
                }

                //von oben links nach oben rechts
                for(int c = i_links; c<=i_rechts ; ++c)
                {
                    if(m_img->getPixel(c,b_oben)==FXRGB(0,0,0)) black = black+1;
                    if(black>0)break;
                }

                //von unten links nach unten rechts
                for(int e = i_links ; e<=i_rechts ; ++e)
                {
                    if(m_img->getPixel(e,b_unten)==FXRGB(0,0,0)) black = black+1;
                    if(black>0)break;
                }

                //von oben rechts nach unten rechts
                for(int f = b_oben+1 ; f<=b_unten-1 ; ++f)
                {
                    if(m_img->getPixel(i_rechts,f)==FXRGB(0,0,0)) black = black+1;
                    if(black>0)break;
                }

                //falls kein schwarzer pixel gefunden wurde,
                //so färbe das gewählte quadrat weiß
                if(black==0)
                    for(int x = i_links+1; x<i_rechts ; ++x)
                        for(int y = b_oben+1 ; y<b_unten; ++y)
                            m_img->setPixel(x,y,FXRGB(255,255,255));
            }
        }

    }
}

void
Image::CreatePoint(Point p, bool cross)
{
    //Hole die Koordinaten des Punktes
    int i=p.GetX();
    int j=p.GetY();

    //Durchlaufe alle umliegenden Pixel und färbe sie schwarz
    for (int k=i-1;k<=i+1;k++)
        for (int l=j-1;l<=j+1;l++) m_img->setPixel(k,l,FXRGB(0,0,0));
}

void
Image::DrawLine(Point p1,Point p2)
    {
    //Hole die Koordinaten der Punkte
    int i=p1.GetX();
    int j=p1.GetY();
    int k=p2.GetX();
    int l=p2.GetY();

    //Variablen für die Koordinatendifferenzen
    int delta_x,delta_y,min_x,min_y,new_x,new_y,deltadelta;

    //Überprüfe die Lage der Strecke (p1,p2) in der Ebene

    //Ist x-Koordinate von p1 kleiner als von p2 ?
    if (i-k<0)
        {
        delta_x=k-i;
        min_x=i;
        deltadelta=-1;
        }

    else
        {
        delta_x=i-k;
        min_x=k;
        deltadelta=1;
        }

    //Ist y-Koordinate von p1 kleiner als von p2 ?
    if (j-l<0)
        {
        delta_y=l-j;
        min_y=j;
        deltadelta*=-1;
        }

    else
        {
        delta_y=j-l;
        min_y=l;
        deltadelta*=1;
        }

    //Es ergibt sich ein deltadelta von 1(-1) bei positiver(negativer) Steigung der Strecke(p1,p2)

    //Fange Integer divide by zero ab
    if(delta_x==0)delta_x=1;

    //Berechne nun mittels der Steigung die Punkte der Strecke (p1,p2) und färbe diese

    //Wird von p1 nach p2 mehr x-Weg zurückgelegt ?
    //Wenn x, dann berechne die Zwischenwerte der y-Koordinaten für jedes delta_x Pixel
    if (delta_x>=delta_y)
        //negative Steigung
        if (deltadelta==-1)
            for (int a=0;a<=delta_x;a++)
                {
                new_y=int(min_y+deltadelta*a*delta_y/delta_x);
                m_img->setPixel(a+min_x,new_y+delta_y,FXRGB(50,0,250));
                }
        //positive Steigung
        else
            for (int a=0;a<=delta_x;a++)
                {
                new_y=int(min_y+a*delta_y/delta_x);
                m_img->setPixel(a+min_x,new_y,FXRGB(50,0,250));
                }
    //Wird von p1 nach p2 mehr y-Weg zurückgelegt ?
    //Wenn y, dann berechne die Zwischenwerte der x-Koordinaten für jedes delta_y Pixel
    else
        //negative Steigung
        if (deltadelta==-1)
            for (int a=0;a<=delta_y;a++)
                {
                new_x=int(min_x+delta_x+deltadelta*a*delta_x/delta_y);
                m_img->setPixel(new_x,a+min_y,FXRGB(50,0,250));
                }
        //positive Steigung
        else
            for (int a=0;a<=delta_y;a++)
                {
                new_x=int(min_x+a*delta_x/delta_y);
                m_img->setPixel(new_x,a+min_y,FXRGB(50,0,250));
                }
}

//Zeichnet das Knotenarray auf das aktuelle Bitmap
void
Image::DrawVArray(Graph gr)
{
    //Hole Breite und Höhe des Image
	int wid = m_img->getWidth();
	int hei = m_img->getHeight();
	
	unsigned int l=0;
    //Hole das Knotenarray
    vector<Vertex*> testarray=gr.GetVArray();

    //Durchlaufe das Knotenarray
    while (l<testarray.size())
    {
        //Hole dir jeweils den Pointer auf den Knoten an Stelle l im Knotenarray
        Vertex* ptemp=testarray[l];

        //Fange Access Violation(Zugriff auf unerlaubte Pixel) ab
        if (((ptemp->GetX()<wid-1)&&(ptemp->GetY()<hei-1))&&((ptemp->GetX()>0)&&(ptemp->GetY()>0)))
        {
        //Erzeuge einen Punkt(3Mal3 Pixel) und male ihn
        Point b(ptemp->GetX(),ptemp->GetY());
        CreatePoint(b,0);
        }
        l++;
    }
}

//Zeichnet das Kantenarray auf das aktuelle Bitmap
void
Image::DrawEArray(Graph gr)
{
    unsigned int n=0;

    //Hole das Kantenarray
    vector<Edge*> testearray=gr.GetEArray();

    //Durchlaufe das Kantenarray
    while (n<testearray.size())
    {
        //Hole dir jeweils den Pointer auf die Kante an Stelle n im Kantenarray
        Edge* ptemp = testearray[n];

        //Hole dir jeweils den Pointer auf den Start- und Endknoten der Kante
        Vertex* a = ptemp->GetStartingVertex();
        Vertex* b = ptemp->GetEndingVertex();

        //Wenn a und b nicht leer sind
        if ((a != NULL)&&(b != NULL))
        {
            //Erzeuge zwei Punkte und male die Strecke dazwischen
            Point aa(a->GetX(),a->GetY());
            Point bb(b->GetX(),b->GetY());
            DrawLine(aa,bb);
        }
        n++;
    }
}

//Zeichnet den Graphen auf das aktuelle Bitmap
void
Image::DrawGraph(Graph gr)
{
    // Knoten und Kanten werden gezeichnet
    DrawEArray(gr);
    DrawVArray(gr);
}

//Siehe oben
void
Image::DrawLine(FXint i,FXint j,FXint k,FXint l)
    {
    int delta_x,delta_y,min_x,min_y,new_x,new_y,deltadelta;

    if (i-k<0)
        {
        delta_x=k-i;
        min_x=i;
        deltadelta=-1;
        }

    else
        {
        delta_x=i-k;
        min_x=k;
        deltadelta=1;
        }

    if (j-l<0)
        {
        delta_y=l-j;
        min_y=j;
        deltadelta*=-1;
        }

    else
        {
        delta_y=j-l;
        min_y=l;
        deltadelta*=1;
        }

    if(delta_x==0)
        delta_x=1;

    if (delta_x>=delta_y)
        if (deltadelta==-1)
            for (int a=0;a<=delta_x;a++)
                {
                new_y=int(min_y+deltadelta*a*delta_y/delta_x);
                m_img->setPixel(a+min_x,new_y+delta_y,FXRGB(0,0,0));
                }
        else
            for (int a=0;a<=delta_x;a++)
                {
                new_y=int(min_y+a*delta_y/delta_x);
                m_img->setPixel(a+min_x,new_y,FXRGB(0,0,0));
                }
    else
        if (deltadelta==-1)
            for (int a=0;a<=delta_y;a++)
                {
                new_x=int(min_x+delta_x+deltadelta*a*delta_x/delta_y);
                m_img->setPixel(new_x,a+min_y,FXRGB(0,0,0));
                }
        else
            for (int a=0;a<=delta_y;a++)
                {
                new_x=int(min_x+a*delta_x/delta_y);
                m_img->setPixel(new_x,a+min_y,FXRGB(0,0,0));
                }
}

//
// Count the Number of 0/1 Transitions of the ordered sequence p2,p3,..p8,p9,p2
// where the indices of the neighbourpixels are given by the following scheme
//  ------------------------------------------
//  |p9=(i-1,j-1) |p2=(i,j-1)  |p3=(i+1,j-1) |
//  |-------------|------------|-------------|
//  |p8=(i-1,j)   |p1=(i,j)    |p4=(i+1,j)   |
//  |-------------|------------|-------------|
//  |p7=(i-1,j+1) |p6=(i,j+1)  |p5=(i+1,j+1) |
//  ------------------------------------------
//
int Image::CountTransitions(int i, int j)
{
    int transitions =0;
        if(
            (m_img->getPixel(i,j-1)==FXRGB(255,255,255))&&
            (m_img->getPixel(i+1,j-1)==FXRGB(0,0,0))
        )
            ++transitions;
        if(
            (m_img->getPixel(i+1,j-1)==FXRGB(255,255,255))&&
            (m_img->getPixel(i+1,j)==FXRGB(0,0,0))
         )
            ++transitions;
        if(
            (m_img->getPixel(i+1,j)==FXRGB(255,255,255))&&
            (m_img->getPixel(i+1,j+1)==FXRGB(0,0,0))
         )
            ++transitions;
        if(
            (m_img->getPixel(i+1,j+1)==FXRGB(255,255,255))&&
            (m_img->getPixel(i,j+1)==FXRGB(0,0,0))
        )
            ++transitions;
        if(
            (m_img->getPixel(i,j+1)==FXRGB(255,255,255))&&
            (m_img->getPixel(i-1,j+1)==FXRGB(0,0,0))
         )
            ++transitions;
        if(
            (m_img->getPixel(i-1,j+1)==FXRGB(255,255,255))&&
            (m_img->getPixel(i-1,j)==FXRGB(0,0,0))
        )
            ++transitions;
        if(
            (m_img->getPixel(i-1,j)==FXRGB(255,255,255))&&
            (m_img->getPixel(i-1,j-1)==FXRGB(0,0,0))
        )
            ++transitions;
        if(
            (m_img->getPixel(i-1,j-1)==FXRGB(255,255,255))&&
            (m_img->getPixel(i,j-1)==FXRGB(0,0,0))
        )
            ++transitions;
    return transitions;
}




