
    /* limvds(vnew,vold)
     * limit the per-iteration change of VDS
     * (From Spice3)
     */

double
limvds(vnew,vold)
    double vnew;
    double vold;

{

    if(vold >= 3.5) {
        if(vnew > vold) {
            vnew = MIN(vnew,(3 * vold) +2);
        } else {
            if (vnew < 3.5) {
                vnew = MAX(vnew,2);
            }
        }
    } else {
        if(vnew > vold) {
            vnew = MIN(vnew,4);
        } else {
            vnew = MAX(vnew,-.5);
        }
    }
    return(vnew);
}
