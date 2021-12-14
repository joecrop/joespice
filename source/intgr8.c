void intgr8(x,xdot,h,alpha,beta)
  double x;
  double xdot;
  double h;
  double *alpha;
  double *beta;
{
  /* TR method */
  *alpha = 2.0/h;
  *beta = (-2.0/h * x - xdot);

  /* BE method */
  //*alpha = 1.0/h;
  //*beta = -1.0/h * x;

}



