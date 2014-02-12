/** \include Example for using of mmg3dlib
 * mmg3d: 3d mesh adaptation
 *
 * Written by Cecile Dobrzynski (IMB), Charles Dapogny and Pascal Frey (LJLL)
 * Copyright (c) 2004- IMB/LJLL.
 * All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>

#include "libmmg3d5.h"

mytime    ctim[TIMEMAX];

#ifdef SINGUL
#define RETURN_AND_FREE(mesh,met,sing,val)do    \
    {                                                 \
      MMG5_Free_all(mesh,met,sing);                   \
      return(val);                                    \
    }while(0)
#else
#define RETURN_AND_FREE(mesh,met,sing,val)do    \
    {                                                 \
      MMG5_Free_all(mesh,met);                        \
      return(val);                                    \
    }while(0)
#endif

static inline void endcod() {
  char    stim[32];

  chrono(OFF,&ctim[0]);
  printim(ctim[0].gdif,stim);
  fprintf(stdout,"\n   MMG3DLIB: ELAPSED TIME  %s\n",stim);
}

int main(int argc,char *argv[]) {
  MMG5_pMesh      mesh;
  MMG5_pSol       met;
  MMG5_pSingul    sing;
  int             ier;
  char            stim[32];

  atexit(endcod);

  tminit(ctim,TIMEMAX);
  chrono(ON,&ctim[0]);

  /* assign default values */
  mesh = NULL;
  met  = NULL;
  sing = NULL;

#ifndef SINGUL
  MMG5_Init_mesh(&mesh,&met);
  //  memset(&sing,0,sizeof(MMG5_Singul));
#else
  MMG5_Init_mesh(&mesh,&met,&sing);
#endif
  /* reset default values for file names */
#ifndef SINGUL
  MMG5_Free_names(mesh,met);
#else
  MMG5_Free_names(mesh,met,sing);
#endif

  /* command line */
#ifdef SINGUL
  if ( !MMG5_parsar(argc,argv,mesh,met,sing) )  return(1);
#else
  if ( !MMG5_parsar(argc,argv,mesh,met) )  return(1);
#endif

  /* load data */
  fprintf(stdout,"\n  -- INPUT DATA\n");
  chrono(ON,&ctim[1]);
  /* read mesh file */
  if ( !MMG5_loadMesh(mesh) ) {
    MMG5_Free_all(mesh,met
#ifdef SINGUL
                 ,sing
#endif
                 );
    return(MMG5_STRONGFAILURE);
  }
  if ( !MMG5_Set_solSize(mesh,met,MMG5_Vertex,0,MMG5_Scalar) ) {
    MMG5_Free_all(mesh,met
#ifdef SINGUL
                  ,sing
#endif
                  );
    return(MMG5_STRONGFAILURE);
  }

  /* read metric if any */
  ier = MMG5_loadMet(mesh,met);
  if ( !ier ) {
    MMG5_Free_all(mesh,met
#ifdef SINGUL
                 ,sing
#endif
                 );
    return(MMG5_STRONGFAILURE);
  }
#ifdef SINGUL
  if ( mesh->info.sing && sing->namein ) {
    ier = MMG5_loadSingul(mesh,sing);
    if ( !ier ) {
      MMG5_Free_all(mesh,met,sing);
      return(MMG5_STRONGFAILURE);
    }
  }
#endif
  if ( !MMG5_parsop(mesh,met) )
    RETURN_AND_FREE(mesh,met,sing,MMG5_LOWFAILURE);

  chrono(OFF,&ctim[1]);
  printim(ctim[1].gdif,stim);
  fprintf(stdout,"  -- DATA READING COMPLETED.     %s\n",stim);

  ier = MMG5_mmg3dlib(mesh,met
#ifdef SINGUL
                      ,sing
#endif
                      );

  if ( ier != MMG5_STRONGFAILURE ) {
    chrono(ON,&ctim[1]);
    if ( mesh->info.imprim )
      fprintf(stdout,"\n  -- WRITING DATA FILE %s\n",mesh->nameout);
    if ( !MMG5_saveMesh(mesh) )         {
      MMG5_Free_all(mesh,met
#ifdef SINGUL
                   ,sing
#endif
                   );
      return(EXIT_FAILURE);
    }
    if ( !MMG5_saveMet(mesh,met) )     {
      MMG5_Free_all(mesh,met
#ifdef SINGUL
                   ,sing
#endif
                   );
      return(EXIT_FAILURE);
    }
    chrono(OFF,&ctim[1]);
    if ( mesh->info.imprim )
      fprintf(stdout,"  -- WRITING COMPLETED\n");
  }

  /* free mem */
  chrono(OFF,&ctim[0]);
  printim(ctim[0].gdif,stim);
  fprintf(stdout,"\n   MMG3D: ELAPSED TIME  %s\n",stim);
  MMG5_Free_all(mesh,met
#ifdef SINGUL
               ,sing
#endif
               );
  return(ier);
}