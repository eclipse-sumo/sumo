#include <string>
#include <qdialog.h>
#include <qevent.h>
#include <qrect.h>
#include <utils/convert/ToString.h>
#include <utils/common/StringUtils.h>
#include "GUIParameterTracker.h"
#include <gui/GUIGlObject.h>
#include <gui/GUIApplicationWindow.h>
#include <guisim/GUINet.h>


using namespace std;

GUIParameterTracker::GUIParameterTracker( GUIApplicationWindow *app,
        QWidget * parent )
        : myApplication(app)
{
    setCaption("Tracker");
    setBaseSize(200, 300);
    setMinimumSize(200, 300);
    app->addChild(this, true);
    myPanel = new
        GUIParameterTrackerPanel(myApplication, this);
    myPanel->setGeometry(QRect( 0, 0, 200, 300));
    show();
}


GUIParameterTracker::GUIParameterTracker( GUIApplicationWindow *app,
        QWidget * parent, GUIGlObject *o, size_t itemNo )
        : myApplication(app)
{
    setCaption("Tracker");
    setBaseSize(200, 300);
    setMinimumSize(200, 300);
    app->addChild(this, true);
    myPanel = new
        GUIParameterTrackerPanel(myApplication, this);
    myPanel->setGeometry(QRect( 0, 0, 200, 300));
    addVariable(o, itemNo);
    show();
}


GUIParameterTracker::~GUIParameterTracker()
{
    myApplication->removeChild(this);
}


void
GUIParameterTracker::addVariable( GUIGlObject *o, size_t itemNo )
{
    TrackerValueDesc *newTracked =
        new TrackerValueDesc(
            o->getFullName() + string(o->getTableItem(itemNo)),
            RGBColor(1, 0, 0.5),
            o, itemNo);
    myTracked.push_back(newTracked);

}


bool
GUIParameterTracker::event ( QEvent *e )
{
    if(e->type()!=QEvent::User) {
        return QDialog::event(e);
    }
    for(TrackedVarsVector::iterator i=myTracked.begin(); i!=myTracked.end(); i++) {
        TrackerValueDesc *desc = *i;
        desc->simStep();
    }
    repaint();
    return TRUE;
}


int
GUIParameterTracker::getMaxGLWidth() const
{
    return myApplication->getMaxGLWidth();
}


int
GUIParameterTracker::getMaxGLHeight() const
{
    return myApplication->getMaxGLHeight();
}


void
GUIParameterTracker::paintEvent ( QPaintEvent *e )
{
    myPanel->paintEvent(e);
    QDialog::paintEvent(e);
}


void
GUIParameterTracker::resizeEvent ( QResizeEvent *e )
{
    myPanel->setGeometry(QRect( 0, 0,
		e->size().width(), e->size().height()));
	QDialog::resizeEvent(e);
}







GUIParameterTracker::GUIParameterTrackerPanel::GUIParameterTrackerPanel(GUIApplicationWindow *app,
                                                                        GUIParameterTracker *parent)
    : QGLWidget(parent), myApplication(app), myParent(*parent),
    _noDrawing(0)
{
}

GUIParameterTracker::GUIParameterTrackerPanel::~GUIParameterTrackerPanel()
{
}


void
GUIParameterTracker::GUIParameterTrackerPanel::initializeGL()
{
    _lock.lock();
    _widthInPixels = myParent.getMaxGLWidth();
    _heightInPixels = myParent.getMaxGLHeight();
    glViewport( 0, 0, myParent.getMaxGLWidth()-1, myParent.getMaxGLHeight()-1 );
    glClearColor( 1.0, 1.0, 1.0, 1 );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );
    glDisable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND );
    glEnable(GL_ALPHA_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glLineWidth(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // !!!
    if(myApplication->myFonts.has("std")) {
        myFontRenderer.add(myApplication->myFonts.get("std"));
    }
    _lock.unlock();
}


void
GUIParameterTracker::GUIParameterTrackerPanel::resizeGL( int width, int height )
{
    _lock.lock();
    _widthInPixels = width;
    _heightInPixels = height;
    glViewport( 0, 0, _widthInPixels, _heightInPixels );
    _lock.unlock();
}

void
GUIParameterTracker::GUIParameterTrackerPanel::paintGL()
{
    _lock.lock();
    // return if the canvas is not visible
    if(!isVisible()) {
		_lock.unlock();
        return;
    }
    _noDrawing++;
    // ...and return when drawing is already being done
    if(_noDrawing>1) {
        _noDrawing--;
        _lock.unlock();
        return;
    }
    _widthInPixels = width();
    _heightInPixels = height();
    // draw
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    drawValues();
    _noDrawing--;
    glFlush();
    swapBuffers();
    _lock.unlock();
}


void
GUIParameterTracker::GUIParameterTrackerPanel::drawValues()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_TEXTURE_2D);
    size_t run = 0;
    for(TrackedVarsVector::iterator i=myParent.myTracked.begin(); i!=myParent.myTracked.end(); i++) {
        TrackerValueDesc *desc = *i;
        drawValue(*desc,
            (float) _widthInPixels / (float) myParent.myTracked.size() * (float) run);
        run++;
    }
    myFontRenderer.Draw(_widthInPixels, _heightInPixels);//this, width, height);
}


void
GUIParameterTracker::GUIParameterTrackerPanel::drawValue(TrackerValueDesc &desc,
                                                         float namePos)
{
    // apply scaling
    glPushMatrix();

    // apply the positiopn offset of the display
    glScaled(0.8, 0.8, 1);
    // apply value range scaling
    double ys = 2.0 / desc.getRange();
    glScaled(1.0, ys, 1.0);
    glTranslated(-1.0, -desc.getYCenter(), 0);

    // set color
    const RGBColor &col = desc.getColor();
    float red = col.red();
    float green = col.green();
    float blue = col.blue();
    // draw value bounderies
    glBegin( GL_LINES );
    // draw value bounderies and descriptions
    glColor4f(red, green, blue, 0.5f);
    myFontRenderer.SetColor(red*0.5f, green*0.5f, blue*0.5f);
        // draw minimum boundery
    glVertex2d(0, desc.getMin());
    glVertex2d(2.0, desc.getMin());
    myFontRenderer.StringOut(0,
        patchHeightVal(desc, desc.getMin()),
        StringUtils::trim(desc.getMin(), 2));
        // draw maximum boundery
    glVertex2d(0, desc.getMax());
    glVertex2d(2.0, desc.getMax());
    myFontRenderer.StringOut(0,
        patchHeightVal(desc, desc.getMax()),
        StringUtils::trim(desc.getMax(), 2));
        // draw some further lines
    glColor4f(red, green, blue, 0.3f);
    for(int a=1; a<6; a++) {
        double ypos = (desc.getRange()) / 6.0 * (double) a + desc.getMin();
        glVertex2d(0, ypos);
        glVertex2d(2.0, ypos);
    }
    glEnd();

    const std::vector<float> &values = desc.getValues();
    if(values.size()<2) {
        glPopMatrix();
        return;
    }

    // init values
    double xStep = 2.0 / ((double) values.size());
    std::vector<float>::const_iterator i = values.begin();
    double yp = (*i++);
    double xp = 0;
    // draw lines
    glBegin( GL_LINES );
    // set color
    glColor4f(red, green, blue, 1.0f);
    for(; i!=values.end(); i++) {
        double yn = (*i);
        double xn = xp + xStep;
        glVertex2d(xp, yp);
        glVertex2d(xn, yn);
        yp = yn;
        xp = xn;
    }
    glEnd();
    glPopMatrix();
    // add current value string
    myFontRenderer.SetColor(red, green, blue);
    myFontRenderer.StringOut(0,
        patchHeightVal(desc, yp),
        StringUtils::trim(yp, 2));
    myFontRenderer.StringOut(namePos, 0,
        desc.getName());
}


float
GUIParameterTracker::GUIParameterTrackerPanel::patchHeightVal(TrackerValueDesc &desc,
                                                              double d)
{
    float height = (float) _heightInPixels;
    float range = (float) desc.getRange();
    float yoff = (float) desc.getYCenter();
    float max = (float) desc.getMax();
    float abs = (height) * (((float)d-yoff)/range) * 0.8f;
    return (height * 0.5f) - abs - 6;
}







