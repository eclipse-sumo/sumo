from os import system 
import matplotlib
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.collections import PolyCollection


#from mpl_toolkits.axes_grid import make_axes_locatable

##############################################################################
# matplotlib configuration

linewidth=2.0
fontsize =12


params = {#'backend': 'ps',
          'axes.labelsize': fontsize,
          #'text.fontsize': fontsize,
          'legend.fontsize': 0.9*fontsize,
          'xtick.labelsize': 0.9*fontsize,
          'ytick.labelsize': 0.9*fontsize,
          'text.usetex': False,
          #'figure.figsize': fig_size
          }

matplotlib.rcParams.update(params)

markers=['o','s','^','d','v','*','h','<','>']
markersize=8
nodesize=1000
##############################################################################
def init_plot(is_tight_layout = False, ind_fig = 0, **kwargs):
    plt.close("all")
    fig = plt.figure(ind_fig,**kwargs)
    ax = fig.add_subplot(111)
    if is_tight_layout:
        fig.tight_layout()
    return ax

def new_plot(is_tight_layout = False, ind_fig = 0):
    
    fig = plt.figure(ind_fig)
    ax = fig.add_subplot(111)
    if is_tight_layout:
        fig.tight_layout()
    ind_fig += 1
    return ax, ind_fig




    
def save_fig(figname, is_adjust_border = False):
  #ffigname = figname+".png"
  #plt.savefig(ffigname,format='PNG')
  
  ffigname = figname+".pdf"
  if is_adjust_border:
    plt.subplots_adjust(left=0.12, bottom=0.1, right=0.86, top=0.9, wspace=0.2, hspace=0.2)
  
  
  plt.savefig(figname+".pdf",format='PDF')
  #plt.savefig(figname+".eps",format='eps',transparent=True)
  #system("ps2pdf -dEPSCrop "+figname+".eps "+figname+".pdf")
  #system("rm "+figname+".eps")
  return ffigname
