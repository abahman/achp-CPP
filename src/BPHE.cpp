#include "CPState.h"
#include "BPHE.h"

#include <algorithm>

//class PHEHXClass():
    
    /*def OutputList(self):
        """
            Return a list of parameters for this component for further output
            
            It is a list of tuples, and each tuple is formed of items:
                [0] Description of value
                [1] Units of value
                [2] The value itself
        """
        #TODO: fix this list of outputs
        return [
            ('Effective Length','m',self.Lp),
            ('Outlet Superheat','K',self.Tin_c-self.Tdew_c),
            ('Q Total','W',self.Q),
            ('Q Superheat Hot','W',self.Q_superheated_h),
            ('Q Two-Phase Hot','W',self.Q_2phase_h),
            ('Q Subcooled Hot','W',self.Q_subcooled_h),
            ('Q Superheat Cold','W',self.Q_superheated_c),
            ('Q Two-Phase Cold','W',self.Q_2phase_c),
            ('Q Subcooled Cold','W',self.Q_subcooled_c),
            ('Inlet hot stream temp','K',self.Tin_h),
            ('Outlet hot stream temp','K',self.Tout_h),
            ('Inlet cold stream temp','K',self.Tin_c),
            ('Outlet cold stream temp','K',self.Tout_c),
            ('Charge Total','kg',self.Charge_h),
            ('Charge Superheat','kg',self.Charge_superheated_h),
            ('Charge Two-Phase','kg',self.Charge_2phase_h),
            ('Charge Subcool','kg',self.Charge_subcooled_h),
            ('Charge Total','kg',self.Charge_c),
            ('Charge Superheat','kg',self.Charge_superheated_c),
            ('Charge Two-Phase','kg',self.Charge_2phase_c),
            ('Charge Subcool','kg',self.Charge_subcooled_c),
            ('Hot HTC Superheat','W/m^2-K',self.h_superheated_h),
            ('Hot HTC Two-Phase','W/m^2-K',self.h_2phase_h),
            ('Hot HTC Subcool','W/m^2-K',self.h_subcooled_h),
            ('Cold Mean HTC Superheat','W/m^2-K',self.h_superheated_c),
            ('Cold Mean HTC Ref. Two-Phase','W/m^2-K',self.h_2phase_c),
            ('Cold Mean HTC Ref. Subcool','W/m^2-K',self.h_subcooled_c),
            ('Pressure Drop Hot','Pa',self.DP_h),
            ('Pressure Drop Hot superheated','Pa',self.DP_superheated_h),
            ('Pressure Drop Hot 2 phase','Pa',self.DP_2phase_h),
            ('Pressure Drop Hot subcooled','Pa',self.DP_subcooled_h),
            ('Pressure Drop Cold','Pa',self.DP_c),
            ('Pressure Drop Cold superheated','Pa',self.DP_superheated_c),
            ('Pressure Drop Cold 2 phase','Pa',self.DP_2phase_c),
            ('Pressure Drop Cold subcooled','Pa',self.DP_subcooled_c),
            ('Area fraction Superheat Hot','-',self.w_superheated_h),
            ('Area fraction Two-Phase Hot','-',self.w_2phase_h),
            ('Area fraction Subcooled Hot','-',self.w_subcooled_h),
            ('Area fraction Superheat Cold','-',self.w_superheated_c),
            ('Area fraction Two-Phase Cold','-',self.w_2phase_c),
            ('Area fraction Subcooled Cold','-',self.w_subcooled_c)
         ]*/
        
void BrazedPlateHeatExchanger::calculate()
{
	this->DetermineQmax();
};
double BrazedPlateHeatExchanger::DetermineQmax()
{
	double Qmax;
    // See if each phase could change phase if it were to reach the
    // inlet temperature of the opposite phase 

	CoolPropStateClass State_h_max = this->State_h_inlet, State_c_max = this->State_c_inlet;
    
	// *****************  EXTERNAL PINCHING ************************
    // Find the maximum possible rate of heat transfer as the minimum of 
    // taking each stream to the inlet temperature of the other stream 
	// at constant pressure
	State_h_max.update(iT, State_c_inlet.T(), iP, State_h_inlet.p());
	State_c_max.update(iT, State_h_inlet.T(), iP, State_c_inlet.p());
  
	// The maximum possible heat transfer rate considering just the external pinching
	double Qhot_max = this->mdot_h*(State_h_inlet.h() - State_h_max.h());
	double Qcold_max = this->mdot_c*(State_c_max.h() - State_c_inlet.h());
	Qmax = std::min(Qhot_max, Qcold_max);

	// If neither of the fluids changes phase in going to the inlet temperature
	// of the other phase, we have already found the maximum heat transfer rate bound
	if ((State_h_inlet.phase() == State_h_max.phase()) 
		&&
		(State_c_inlet.phase() == State_c_max.phase()) )
	{
		return Qmax;
	}

	// **************** INTERNAL PINCHING *********************
	/*
	Because we have made it this far, it means that there is the possibility that 
	one or both of the fluids might change phase in the BPHE.  Even so, at this
	point, it is still possible that neither fluid will change phase if the 
	fluid that is *not* the limiting Q would change phase in going to the inlet
	state of the other fluid
	*/

	double rr = 0;
    
    // Now we need to check for internal pinch points where the temperature
    // profiles would tend to overlap given the "normal" definitions of 
    // maximum heat transfer of taking each stream to the inlet temperature 
    // of the other stream
    //
    // First we build the same vectors of enthalpies like below
    //EnthalpyList_c,EnthalpyList_h=self.BuildEnthalpyLists(Qmax)
    
	// Then we find the temperature of each stream at each junction
//    TList_c=np.zeros_like(EnthalpyList_c)
//    TList_h=np.zeros_like(EnthalpyList_h)
//
//    if not len(EnthalpyList_h)==len(EnthalpyList_c):
//        raise ValueError('Length of enthalpy lists for both fluids must be the same')
//    
//    // Make the lists of temperatures of each fluid at each cell boundary
//    for i in range(len(EnthalpyList_h)):
//        TList_c[i] = TrhoPhase_ph(self.Ref_c,self.pin_c,EnthalpyList_c[i],self.Tbubble_c,self.Tdew_c,self.rhosatL_c,self.rhosatV_c)[0]
//        TList_h[i] = TrhoPhase_ph(self.Ref_h,self.pin_h,EnthalpyList_h[i],self.Tbubble_h,self.Tdew_h,self.rhosatL_h,self.rhosatV_h)[0]
//    
//    // TODO: could do with more generality if both streams can change phase
//    // Check if any internal points are pinched
////#        if np.sum(TList_c>TList_h)>0:
////#            #Loop over the internal cell boundaries
////#            for i in range(1,len(TList_c)-1):
////#                #If cold stream is hotter than the hot stream
////#                if TList_c[i]-1e-9>TList_h[i]:
////#                    #Find new enthalpy of cold stream at the hot stream cell boundary
////#                    hpinch=Props('H','T',TList_h[i],'P',self.pin_c,self.Ref_c)*1000
////#                    #Find heat transfer of hot stream in right-most cell
////#                    Qextra=self.mdot_h*(EnthalpyList_h[i+1]-EnthalpyList_h[i])
////#                    Qmax=self.mdot_c*(hpinch-self.hin_c)+Qextra
//      
//    #Brandon's version for general derating of Qmax (handles hot fluid, cold fluid, or dual pinch points)
//    #Replaces above lines up to "if np.sum(TList_c..."
//    Qmax_new = None
//
//    if (TList_c[0]+1e-6 < self.Tbubble_c <= TList_c[-1] and TList_h[0] <= self.Tdew_h < TList_h[-1]-1e-6) \
//    and ((self.Tbubble_c > TList_h[1]) and (self.Tdew_h < TList_c[-2])): #both fluids change phase AND dual pinch points
//        hpinch_h = Props('H','T',self.Tbubble_c,'P',self.pin_h,self.Ref_h)*1000
//        hpinch_c = Props('H','T',self.Tdew_h,'P',self.pin_c,self.Ref_c)*1000
//        Qmax_h = self.mdot_h*(self.hsatV_h - hpinch_h)
//        Qmax_c = self.mdot_c*(hpinch_c - self.hsatL_c)
//        Qmax_new = min(self.mdot_h*(self.hsatV_h - hpinch_h), self.mdot_c*(hpinch_c - self.hsatL_c)) + self.mdot_c*(self.hsatL_c - self.hin_c) + self.mdot_h*(self.hin_h - self.hsatV_h)
//        Qmax_new_check = min(self.mdot_h*(self.hin_h - hpinch_h) + self.mdot_c*(self.hsatL_c - self.hin_c), 
//                             self.mdot_c*(hpinch_c - self.hin_c) + self.mdot_h*(self.hin_h - self.hsatV_h))
//        
//        print 'Qmax_new_check =', Qmax_new_check
//        
//    else: #I had to add the 1e-6 tolerance because otherwise the test could return false.  Checking quality would be better maybe?
//        if TList_c[0]+1e-6 < self.Tbubble_c <= TList_c[-1]+1e-6:  #then there is a cold fluid phase change boundary at the bubble point that could result in a hot fluid pinch point 
//            i = 0
//            while TList_c[i] < self.Tbubble_c-1e-9:  #find the boundary where the cold fluid changes phase
//                i+=1
//            if (self.Tbubble_c > TList_h[i]):  #hot fluid pinch point
//                hpinch = Props('H','T',self.Tbubble_c,'P',self.pin_h,self.Ref_h)*1000
//                Qmax_new = self.mdot_h*(self.hin_h - hpinch) + self.mdot_c*(self.hsatL_c - self.hin_c)
//            
//        if TList_h[0]-1e-6 <= self.Tdew_h < TList_h[-1]-1e-6:  #then there is a hot fluid phase change boundary at the dew point that could result in a cold fluid pinch point
//            i = len(TList_h)-1
//            while TList_h[i] > self.Tdew_h+1e-9:
//                i-=1
//            if (self.Tdew_h < TList_c[i]): #cold fluid pinch point          
//                hpinch = Props('H','T',self.Tdew_h,'P',self.pin_c,self.Ref_c)*1000
//                Qmax_new = self.mdot_c*(hpinch - self.hin_c) + self.mdot_h*(self.hin_h - self.hsatV_h)
//        
//    print 'Qmax =', Qmax
//    print 'Qmax_new =', Qmax_new
//    if Qmax_new:
//        assert(abs(Qmax_new) <= abs(Qmax))
//        Qmax = Qmax_new 
//        EnthalpyList_c,EnthalpyList_h=self.BuildEnthalpyLists(Qmax)  #Rebuild the enthalpy list so we can plot it
//        #Rebuild the temperature list
//        TList_c=np.zeros_like(EnthalpyList_c)
//        TList_h=np.zeros_like(EnthalpyList_h)
//        for i in range(len(EnthalpyList_h)):
//            TList_c[i] = TrhoPhase_ph(self.Ref_c,self.pin_c,EnthalpyList_c[i],self.Tbubble_c,self.Tdew_c,self.rhosatL_c,self.rhosatV_c)[0]
//            TList_h[i] = TrhoPhase_ph(self.Ref_h,self.pin_h,EnthalpyList_h[i],self.Tbubble_h,self.Tdew_h,self.rhosatL_h,self.rhosatV_h)[0]
//     
//    #Here we will plot a qualitative look at what the temperature profiles would be for maximum heat transfer
//    #The abscissa is dimensionless enthalpy so it doesn't show us how long each section is in the physical heat exchanger
//    figure()
//    pseudolength = (np.array(EnthalpyList_c) - EnthalpyList_c[0])/(EnthalpyList_c[-1] - EnthalpyList_c[0])
//    plot(pseudolength, TList_c, label=str.split(self.Ref_c,'.')[0]) 
//    plot(pseudolength, TList_h, label=str.split(self.Ref_h,'.')[0])
//    xlabel('Normalized Enthalpy (a pseudolength) [-]')
//    ylabel('Fluid Temperature [K]')
//    title('Theoretical Maximum Heat Transfer (Pinch Points Checked)')
//    leg=legend(loc='best', fancybox=True)
//    leg.get_frame().set_alpha(0.5)   
//    
//    return Qmax
};
        
//    def PlateHTDP(self,Ref,T,p,mdot_gap):
//        """
//        For single phase fluids, inputs in K, kPa, outputs in W/m^2-K, J/kg-K
//        """
//        Inputs={
//            'Ref':Ref,
//            'T':T,
//            'p':p,
//            'mdot_gap' : mdot_gap,
//            'PlateAmplitude': self.PlateAmplitude,
//            'PlateWavelength' : self.PlateWavelength,
//            'InclinationAngle': self.InclinationAngle,
//            'Bp': self.Bp,
//            'Lp': self.Lp
//        }
//        Outputs=PHE_1phase_hdP(Inputs)
//        return Outputs['h'],Outputs['cp'],Outputs
//    
//    def BuildEnthalpyLists(self,Q):
//        #Start the enthalpy lists with inlet and outlet enthalpies
//        #Ordered from lowest to highest enthalpies for both streams
//        EnthalpyList_h=[self.hin_h-Q/self.mdot_h, self.hin_h]
//        EnthalpyList_c=[self.hin_c,self.hin_c+Q/self.mdot_c]
//        
//        #Save the value of Q and outlet enthalpies
//        self.Q=Q
//        self.hout_h=EnthalpyList_h[0]
//        self.hout_c=EnthalpyList_c[1]
//        
//        #Find the phase boundaries that exist, and add them to lists
//        if IsFluidType(self.Ref_h,'Brine'):
//            hsatL_h=1e9
//            hsatV_h=1e9
//        else:
//            hsatL_h=Props('H','T',self.Tbubble_h,'D',self.rhosatL_h,self.Ref_h)*1000
//            hsatV_h=Props('H','T',self.Tdew_h,'D',self.rhosatV_h,self.Ref_h)*1000
//        
//        if IsFluidType(self.Ref_c,'Brine'):
//            hsatL_c=1e9
//            hsatV_c=1e9
//        else:
//            hsatL_c=Props('H','T',self.Tbubble_c,'D',self.rhosatL_c,self.Ref_c)*1000
//            hsatV_c=Props('H','T',self.Tdew_c,'D',self.rhosatV_c,self.Ref_c)*1000
//        
//        # Check whether the enthalpy boundaries are within the bounds set by 
//        # the imposed amount of heat transfer
//        if hsatV_c<EnthalpyList_c[-1] and hsatV_c>EnthalpyList_c[0]:
//            EnthalpyList_c.insert(len(EnthalpyList_c)-1,hsatV_c)
//        if hsatL_c<EnthalpyList_c[-1] and hsatL_c>EnthalpyList_c[0]:
//            EnthalpyList_c.insert(1,hsatL_c)
//            
//        if hsatV_h<EnthalpyList_h[-1] and hsatV_h>EnthalpyList_h[0]:
//            EnthalpyList_h.insert(len(EnthalpyList_h)-1,hsatV_h)
//        if hsatL_h<EnthalpyList_h[-1] and hsatL_h>EnthalpyList_h[0]:
//            EnthalpyList_h.insert(1,hsatL_h)
//            
//        I_h=0
//        I_c=0
//        while I_h<len(EnthalpyList_h)-1:
//            #Try to figure out whether the next phase transition is on the hot or cold side     
//            Qbound_h=self.mdot_h*(EnthalpyList_h[I_h+1]-EnthalpyList_h[I_h])
//            Qbound_c=self.mdot_c*(EnthalpyList_c[I_c+1]-EnthalpyList_c[I_c])
//            if Qbound_h<Qbound_c-1e-9:
//                # Minimum amount of heat transfer is on the hot side,
//                # add another entry to EnthalpyList_c 
//                EnthalpyList_c.insert(I_c+1, EnthalpyList_c[I_c]+Qbound_h/self.mdot_c)
//            elif Qbound_h>Qbound_c+1e-9:
//                # Minimum amount of heat transfer is on the cold side,
//                # add another entry to EnthalpyList_h at the interface
//                EnthalpyList_h.insert(I_h+1, EnthalpyList_h[I_h]+Qbound_c/self.mdot_h)
//            I_h+=1
//            I_c+=1
//                    
//        self.hsatL_c=hsatL_c
//        self.hsatL_h=hsatL_h
//        self.hsatV_c=hsatV_c
//        self.hsatV_h=hsatV_h
//
//        return EnthalpyList_c,EnthalpyList_h
//    
//    def PostProcess(self,cellList):
//        """
//        Combine all the cells to calculate overall parameters like pressure drop
//        and fraction of heat exchanger in two-phase on both sides
//        """
//        def collect(cellList,tag,tagvalue,out):
//            collectList=[]
//            for cell in cellList:
//                if cell[tag]==tagvalue:
//                    collectList.append(cell[out])
//            return collectList
//        self.DP_c=0
//        self.DP_c_superheat=0
//        self.DP_c_2phase=0
//        self.DP_c_subcooled=0
//        self.DP_h=0
//        self.DP_h_superheat=0
//        self.DP_h_2phase=0
//        self.DP_h_subcooled=0
//        self.Charge_c=0
//        self.Charge_h=0
//        for cell in cellList:
//            self.DP_c+=cell['DP_c']
//            self.DP_h+=cell['DP_h']
//            self.Charge_c+=cell['Charge_c']
//            self.Charge_h+=cell['Charge_h']
//        self.w_superheated_h=sum(collect(cellList,'Phase_h','Superheated','w'))
//        self.w_2phase_h=sum(collect(cellList,'Phase_h','TwoPhase','w'))
//        self.w_subcooled_h=sum(collect(cellList,'Phase_h','Subcooled','w'))
//        self.w_superheated_c=sum(collect(cellList,'Phase_c','Superheated','w'))
//        self.w_2phase_c=sum(collect(cellList,'Phase_c','TwoPhase','w'))
//        self.w_subcooled_c=sum(collect(cellList,'Phase_c','Subcooled','w'))
//        
//        self.DP_superheated_c=sum(collect(cellList,'Phase_c','Superheated','DP_c'))
//        self.DP_2phase_c=sum(collect(cellList,'Phase_c','TwoPhase','DP_c'))
//        self.DP_subcooled_c=sum(collect(cellList,'Phase_c','Subcooled','DP_c'))
//        self.DP_c=self.DP_superheated_c+self.DP_2phase_c+self.DP_subcooled_c
//        
//        self.DP_superheated_h=sum(collect(cellList,'Phase_h','Superheated','DP_h'))
//        self.DP_2phase_h=sum(collect(cellList,'Phase_h','TwoPhase','DP_h'))
//        self.DP_subcooled_h=sum(collect(cellList,'Phase_h','Subcooled','DP_h'))
//        self.DP_h=self.DP_superheated_h+self.DP_2phase_h+self.DP_subcooled_h
//        
//        self.Charge_superheated_c=sum(collect(cellList,'Phase_c','Superheated','Charge_c'))
//        self.Charge_2phase_c=sum(collect(cellList,'Phase_c','TwoPhase','Charge_c'))
//        self.Charge_subcooled_c=sum(collect(cellList,'Phase_c','Subcooled','Charge_c'))
//        self.Charge_c=self.Charge_superheated_c+self.Charge_2phase_c+self.Charge_subcooled_c
//        self.Charge_superheated_h=sum(collect(cellList,'Phase_h','Superheated','Charge_h'))
//        self.Charge_2phase_h=sum(collect(cellList,'Phase_h','TwoPhase','Charge_h'))
//        self.Charge_subcooled_h=sum(collect(cellList,'Phase_h','Subcooled','Charge_h'))
//        self.Charge_h=self.Charge_superheated_h+self.Charge_2phase_h+self.Charge_subcooled_h
//        
//        self.Q_superheated_h=sum(collect(cellList,'Phase_h','Superheated','Q'))
//        self.Q_2phase_h=sum(collect(cellList,'Phase_h','TwoPhase','Q'))
//        self.Q_subcooled_h=sum(collect(cellList,'Phase_h','Subcooled','Q'))
//        self.Q_superheated_c=sum(collect(cellList,'Phase_c','Superheated','Q'))
//        self.Q_2phase_c=sum(collect(cellList,'Phase_c','TwoPhase','Q'))
//        self.Q_subcooled_c=sum(collect(cellList,'Phase_c','Subcooled','Q'))
//        
//        w_superheat=collect(cellList,'Phase_c','Superheated','w')
//        w_2phase=collect(cellList,'Phase_c','TwoPhase','w')
//        h_c_sh=collect(cellList,'Phase_c','Superheated','h_c')
//        h_c_2phase=collect(cellList,'Phase_c','TwoPhase','h_c')
//        self.xout_h=collect(cellList,'Phase_h','TwoPhase','xout_h')
//        
//        if len(w_superheat)>0:
//            self.h_superheated_c=float(sum(np.array(h_c_sh)*np.array(w_superheat))/sum(w_superheat))
//        else:
//            self.h_superheated_c=0
//            
//        if len(w_2phase)>0:
//            self.h_2phase_c=float(sum(np.array(h_c_2phase)*np.array(w_2phase))/sum(w_2phase))
//        else:
//            self.h_2phase_c=0
//            
//        ### Collect all the cells on the hot side
//        w_subcooled_h=collect(cellList,'Phase_h','Subcooled','w')
//        w_superheat_h=collect(cellList,'Phase_h','Superheated','w')
//        w_2phase_h=collect(cellList,'Phase_h','TwoPhase','w')
//        h_h_sh=collect(cellList,'Phase_h','Superheated','h_h')
//        h_h_2phase=collect(cellList,'Phase_h','TwoPhase','h_h')
//        h_h_subcool=collect(cellList,'Phase_h','Subcooled','h_h')
//        
//        w_subcooled_c=collect(cellList,'Phase_c','Subcooled','w')
//        w_superheat_c=collect(cellList,'Phase_c','Superheated','w')
//        w_2phase_c=collect(cellList,'Phase_c','TwoPhase','w')
//        h_c_sh=collect(cellList,'Phase_c','Superheated','h_c')
//        h_c_2phase=collect(cellList,'Phase_c','TwoPhase','h_c')
//        h_c_subcool=collect(cellList,'Phase_c','Subcooled','h_c')
//        
//        if len(w_subcooled_h)>0:
//            self.h_subcooled_h=float(sum(np.array(h_h_subcool)*np.array(w_subcooled_h))/sum(w_subcooled_h))
//        else:
//            self.h_subcooled_h=0
//        
//        if len(w_2phase_h)>0:
//            self.h_2phase_h=float(sum(np.array(h_h_2phase)*np.array(w_2phase_h))/sum(w_2phase_h))
//        else:
//            self.h_2phase_h=0
//            
//        if len(w_superheat_h)>0:
//            self.h_superheated_h=float(sum(np.array(h_h_sh)*np.array(w_superheat_h))/sum(w_superheat_h))
//        else:
//            self.h_superheated_h=0
//            
//        if len(w_subcooled_c)>0:
//            self.h_subcooled_c=float(sum(np.array(h_c_subcool)*np.array(w_subcooled_c))/sum(w_subcooled_c))
//        else:
//            self.h_subcooled_c=0
//        
//        if len(w_2phase_c)>0:
//            self.h_2phase_c=float(sum(np.array(h_c_2phase)*np.array(w_2phase_c))/sum(w_2phase_c))
//        else:
//            self.h_2phase_c=0
//            
//        if len(w_superheat_c)>0:
//            self.h_superheated_c=float(sum(np.array(h_c_sh)*np.array(w_superheat_c))/sum(w_superheat_c))
//        else:
//            self.h_superheated_c=0
//            
//        
//        
//        self.q_flux=collect(cellList,'Phase_c','TwoPhase','q_flux')
//            
//        self.Tout_h,self.rhoout_h=TrhoPhase_ph(self.Ref_h,self.pin_h,self.hout_h,self.Tbubble_h,self.Tdew_h,self.rhosatL_h,self.rhosatV_h)[0:2]
//        self.Tout_c,self.rhoout_c=TrhoPhase_ph(self.Ref_c,self.pin_c,self.hout_c,self.Tbubble_c,self.Tdew_c,self.rhosatL_c,self.rhosatV_c)[0:2]
//        
//        if IsFluidType(self.Ref_c,'Brine'):
//            self.sout_c=Props('S','T',self.Tout_c,'P',self.pin_c,self.Ref_c)*1000
//            self.DT_sc_c=1e9
//        else:
//            self.sout_c=Props('S','T',self.Tout_c,'D',self.rhoout_c,self.Ref_c)*1000
//            #Effective subcooling for both streams
//            hsatL=Props('H','T',self.Tbubble_c,'Q',0,self.Ref_c)*1000
//            cpsatL=Props('C','T',self.Tbubble_c,'Q',0,self.Ref_c)*1000
//            if self.hout_c>hsatL:
//                #Outlet is at some quality on cold side
//                self.DT_sc_c=-(self.hout_c-hsatL)/cpsatL
//            else:
//                self.DT_sc_c=self.Tbubble_c-self.Tout_c
//        
//        if IsFluidType(self.Ref_h,'Brine'):
//            self.sout_h=Props('S','T',self.Tout_h,'P',self.pin_h,self.Ref_h)*1000
//            self.DT_sc_h=1e9
//        else:
//            self.sout_h=Props('S','T',self.Tout_h,'D',self.rhoout_h,self.Ref_h)*1000
//            hsatV=Props('H','T',self.Tdew_h,'Q',0,self.Ref_h)*1000
//            cpsatV=Props('C','T',self.Tdew_h,'Q',0,self.Ref_h)*1000
//            if self.hout_h<hsatV:
//                #Outlet is at some quality on hot side
//                self.DT_sc_h=-(hsatV-self.hout_h)/cpsatV
//            else:
//                self.DT_sc_h=self.Tout_h - self.Tbubble_h
//        
//    def eNTU_CounterFlow(self,Cr,Ntu):
//        return ((1 - exp(-Ntu * (1 - Cr))) / 
//            (1 - Cr * exp(-Ntu * (1 - Cr))))
//    
//    def _OnePhaseH_OnePhaseC_Qimposed(self,Inputs):
//        """
//        Single phase on both sides
//        Inputs is a dict of parameters
//        """
//        
//        #Calculate the mean temperature
//        Tmean_h=Inputs['Tmean_h']
//        Tmean_c=Inputs['Tmean_c']
//        #Evaluate heat transfer coefficient for both fluids
//        h_h,cp_h,PlateOutput_h=self.PlateHTDP(self.Ref_h, Tmean_h, Inputs['pin_h'],self.mdot_h/self.NgapsHot)
//        h_c,cp_c,PlateOutput_c=self.PlateHTDP(self.Ref_c, Tmean_c, Inputs['pin_c'],self.mdot_c/self.NgapsCold)
//        
//        #Use cp calculated from delta h/delta T
//        cp_h=Inputs['cp_h']
//        cp_c=Inputs['cp_c']
//        #Evaluate UA [W/K] if entire HX was in this section 
//        UA_total=1/(1/(h_h*self.A_h_wetted)+1/(h_c*self.A_c_wetted)+self.PlateThickness/(self.PlateConductivity*(self.A_c_wetted+self.A_h_wetted)/2.))
//        #Get Ntu [-]
//        C=[cp_c*self.mdot_c,cp_h*self.mdot_h]
//        Cmin=min(C)
//        Cr=Cmin/max(C)
//        
//        #Effectiveness [-]
//        Q=Inputs['Q']
//        Qmax=Cmin*(Inputs['Tin_h']-Inputs['Tin_c'])
//        epsilon = Q/Qmax
//        
//        if 1 <= epsilon < 1+1e-6:  #if epsilon is slightly larger than 1
//            epsilon = 1-1e-12
//        
//        #Pure counterflow with Cr<1 (Incropera Table 11.4)
//        NTU=1/(Cr-1)*log((epsilon-1)/(epsilon*Cr-1))
//        
//        #Required UA value
//        UA_req=Cmin*NTU
//        
//        #w is required part of heat exchanger for this duty
//        w=UA_req/UA_total
//        
//        #Determine both charge components
//        rho_h=Props('D','T',Tmean_h, 'P', self.pin_h, self.Ref_h)
//        Charge_h = w * self.V_h * rho_h
//        rho_c=Props('D','T',Tmean_c, 'P', self.pin_c, self.Ref_c)
//        Charge_c = w * self.V_c * rho_c
//        
//        #Pack outputs
//        Outputs={
//            'w': w,
//            'Tout_h': Inputs['Tin_h']-Q/(self.mdot_h*cp_h),
//            'Tout_c': Inputs['Tin_c']+Q/(self.mdot_c*cp_c),
//            'Charge_c': Charge_c,
//            'Charge_h': Charge_h,
//            'DP_h': -PlateOutput_h['DELTAP'],
//            'DP_c': -PlateOutput_c['DELTAP'],
//            'h_h':h_h,
//            'h_c':h_c,
//            
//        }
//        return dict(Inputs.items()+Outputs.items())
//    
//    def _OnePhaseH_OnePhaseC_wimposed(self,Inputs):
//        """
//        Single phase on both sides
//        Inputs is a dict of parameters that is the return value of _OnePhaseH_OnePhaseC_Qimposed
//        """
//        w = Inputs['w']
//        #Calculate the mean temperature
//        Tmean_h=Inputs['Tmean_h']  #We don't need to iterate to find the mean temperature because we already know there is more than enough length for this section's effectiveness to be 1.
//        Tmean_c=Inputs['Tmean_c']
//        #Evaluate heat transfer coefficient for both fluids
//        h_h,cp_h,PlateOutput_h=self.PlateHTDP(self.Ref_h, Tmean_h, Inputs['pin_h'],self.mdot_h/self.NgapsHot)
//        h_c,cp_c,PlateOutput_c=self.PlateHTDP(self.Ref_c, Tmean_c, Inputs['pin_c'],self.mdot_c/self.NgapsCold)
//        
//        #Use cp calculated from delta h/delta T
//        cp_h=Inputs['cp_h']
//        cp_c=Inputs['cp_c']
//        #Evaluate UA [W/K] if entire HX was in this section 
//        UA_total=1/(1/(h_h*self.A_h_wetted)+1/(h_c*self.A_c_wetted)+self.PlateThickness/(self.PlateConductivity*(self.A_c_wetted+self.A_h_wetted)/2.))
//        UA_actual = UA_total*w  #We impose length fraction here so we already know the actual UA value
//        #Get Ntu [-] and Effectiveness [-]
//        C=[cp_c*self.mdot_c,cp_h*self.mdot_h]
//        Cmin=min(C)
//        Cr=Cmin/max(C)
//        
//        NTU = UA_actual/Cmin
//        
//        epsilon = (1 - exp(-NTU*(1 - Cr)))/(1 - Cr*exp(-NTU*(1 - Cr)))  #Pure counterflow with Cr<1 (Incropera Table 11.3)
//        assert(epsilon <= 1)
//        
//        Qmax=Cmin*(Inputs['Tin_h']-Inputs['Tin_c'])
//        Q = Qmax*epsilon
//               
//        #Determine both charge components
//        rho_h=Props('D','T',Tmean_h, 'P', self.pin_h, self.Ref_h)
//        Charge_h = w * self.V_h * rho_h
//        rho_c=Props('D','T',Tmean_c, 'P', self.pin_c, self.Ref_c)
//        Charge_c = w * self.V_c * rho_c
//        
//        #Pack outputs
//        Outputs={
//            'Tout_h': Inputs['Tin_h']-Q/(self.mdot_h*cp_h),
//            'Tout_c': Inputs['Tin_c']+Q/(self.mdot_c*cp_c),
//            'Charge_c': Charge_c,
//            'Charge_h': Charge_h,
//            'DP_h': -PlateOutput_h['DELTAP'],
//            'DP_c': -PlateOutput_c['DELTAP'],
//            'h_h':h_h,
//            'h_c':h_c,
//            'Q_wimposed':Q
//        }
//        return dict(Inputs.items()+Outputs.items())  #Overwrites any outputs that were passed back in as inputs with the new outputs
//
//    def _OnePhaseH_TwoPhaseC_Qimposed(self,Inputs):
//        """
//        The hot stream is all single phase, and the cold stream is evaporating
//        """
//        #Calculate the mean temperature for the single-phase fluid
//        h_h,cp_h,PlateOutput_h=self.PlateHTDP(self.Ref_h, Inputs['Tmean_h'], Inputs['pin_h'],self.mdot_h/self.NgapsHot)
//        #Use cp calculated from delta h/delta T
//        cp_h=Inputs['cp_h']
//        #Mole mass of refrigerant for Cooper correlation
//        M=Props('M','T',0,'P',0,self.Ref_c)
//        #Reduced pressure for Cooper Correlation
//        pstar=Inputs['pin_c']/Props('E','T',0,'P',0,self.Ref_c)
//        change=999
//        w=1
//        Q=Inputs['Q']
//        """
//        The Cooper Pool boiling relationship is a function of the heat flux, 
//        therefore the heat flux must be iteratively determined
//        """
//        while abs(change)>1e-6:
//            q_flux=Q/(w*self.A_c_wetted)
//            
//            #Heat transfer coefficient from Cooper Pool Boiling
//            h_c_2phase=Cooper_PoolBoiling(pstar,1.0,q_flux,M) #1.5 correction factor comes from Claesson Thesis on plate HX
//            
//            G=self.mdot_c/self.A_c_flow
//            Dh=self.Dh_c
//            x=(Inputs['xin_c']+Inputs['xout_c'])/2
//
//            UA_total=1/(1/(h_h*self.A_h_wetted)+1/(h_c_2phase*self.A_c_wetted)+self.PlateThickness/(self.PlateConductivity*self.A_c_wetted))
//            C_h=cp_h*self.mdot_h
//            
//            Qmax=C_h*(Inputs['Tin_h']-Inputs['Tsat_c'])
//            epsilon=Q/Qmax
//            
//            if 1 <= epsilon < 1+1e-6:  #if epsilon is slightly larger than 1
//                epsilon = 1-1e-12
//            
//            NTU=-log(1-epsilon)
//            UA_req=NTU*C_h
//            
//            change=UA_req/UA_total-w
//            w=UA_req/UA_total
//        
//        #Refrigerant charge
//        rho_h=Props('D','T',Inputs['Tmean_h'], 'P', self.pin_h, self.Ref_h)
//        Charge_h = w * self.V_h * rho_h
//        rho_c=TwoPhaseDensity(self.Ref_c,Inputs['xin_c'],Inputs['xout_c'],self.Tdew_c,self.Tbubble_c,slipModel='Zivi')
//        Charge_c = rho_c * w * self.V_c
//        
//        #Use Lockhart Martinelli to calculate the pressure drop.  Claesson found good agreement using C parameter of 4.67
//        DP_frict_c=LMPressureGradientAvg(Inputs['xin_c'],Inputs['xout_c'],self.Ref_c,self.mdot_c/self.A_c_flow,self.Dh_c,self.Tbubble_c,self.Tdew_c,C=4.67)*w*self.Lp
//        #Accelerational pressure drop component    
//        DP_accel_c=AccelPressureDrop(Inputs['xin_c'],Inputs['xout_c'],self.Ref_c,self.mdot_c/self.A_c_flow,self.Tbubble_c,self.Tdew_c)
//        
//        #Pack outputs
//        Outputs={
//            'w':w,
//            'Tout_h': Inputs['Tin_h']-Q/(self.mdot_h*cp_h),
//            'Tout_c': Inputs['Tsat_c'],
//            'Charge_c': Charge_c,
//            'Charge_h': Charge_h,
//            'DP_h': -PlateOutput_h['DELTAP'],
//            'DP_c': DP_frict_c+DP_accel_c,
//            'h_h':h_h,
//            'h_c':h_c_2phase,
//            'q_flux':q_flux
//        }
//        return dict(Inputs.items()+Outputs.items())
//    
//    def _OnePhaseH_TwoPhaseC_wimposed(self,Inputs):
//        """
//        The hot stream is all single phase, and the cold stream is evaporating
//        """
//        w = Inputs['w']
//        #Calculate the mean temperature for the single-phase fluid
//        h_h,cp_h,PlateOutput_h=self.PlateHTDP(self.Ref_h, Inputs['Tmean_h'], Inputs['pin_h'],self.mdot_h/self.NgapsHot)
//        #Use cp calculated from delta h/delta T
//        cp_h=Inputs['cp_h']
//        #Mole mass of refrigerant for Cooper correlation
//        M=Props('M','T',0,'P',0,self.Ref_c)
//        #Reduced pressure for Cooper Correlation
//        pstar=Inputs['pin_c']/Props('E','T',0,'P',0,self.Ref_c)
//
//        C_h=cp_h*self.mdot_h
//        Q=C_h*(Inputs['Tin_h']-Inputs['Tsat_c'])  #initial guess for Cooper Pool boiling
//        change = 999
//        """
//        The Cooper Pool boiling relationship is a function of the heat flux, 
//        therefore the heat flux must be iteratively determined.
//        """
//        
//        while abs(change) > 1e-6:
//            q_flux=Q/(w*self.A_c_wetted)
//            
//            #Heat transfer coefficient from Cooper Pool Boiling
//            h_c_2phase=Cooper_PoolBoiling(pstar,1.0,q_flux,M) #1.5 correction factor comes from Claesson Thesis on plate HX
//            
//            G=self.mdot_c/self.A_c_flow
//            Dh=self.Dh_c
//            x=(Inputs['xin_c']+Inputs['xout_c'])/2
//    
//            UA_total=1/(1/(h_h*self.A_h_wetted)+1/(h_c_2phase*self.A_c_wetted)+self.PlateThickness/(self.PlateConductivity*self.A_c_wetted))
//            UA_actual = UA_total*w
//            
//            C_h=cp_h*self.mdot_h
//            NTU = UA_actual/C_h
//            epsilon = 1 - exp(-NTU)
//            
//            Qmax=C_h*(Inputs['Tin_h']-Inputs['Tsat_c'])
//            
//            change=Qmax*epsilon-Q
//            Q = Qmax*epsilon
//        
//        #Refrigerant charge
//        rho_h=Props('D','T',Inputs['Tmean_h'], 'P', self.pin_h, self.Ref_h)
//        Charge_h = w * self.V_h * rho_h
//        rho_c=TwoPhaseDensity(self.Ref_c,Inputs['xin_c'],Inputs['xout_c'],self.Tdew_c,self.Tbubble_c,slipModel='Zivi')
//        Charge_c = rho_c * w * self.V_c
//        
//        #Use Lockhart Martinelli to calculate the pressure drop.  Claesson found good agreement using C parameter of 4.67
//        DP_frict_c=LMPressureGradientAvg(Inputs['xin_c'],Inputs['xout_c'],self.Ref_c,self.mdot_c/self.A_c_flow,self.Dh_c,self.Tbubble_c,self.Tdew_c,C=4.67)*w*self.Lp
//        #Accelerational pressure drop component    
//        DP_accel_c=AccelPressureDrop(Inputs['xin_c'],Inputs['xout_c'],self.Ref_c,self.mdot_c/self.A_c_flow,self.Tbubble_c,self.Tdew_c)
//        
//        #Pack outputs
//        Outputs={
//            'Tout_h': Inputs['Tin_h']-Q/(self.mdot_h*cp_h),
//            'Tout_c': Inputs['Tsat_c'],
//            'Charge_c': Charge_c,
//            'Charge_h': Charge_h,
//            'DP_h': -PlateOutput_h['DELTAP'],
//            'DP_c': DP_frict_c+DP_accel_c,
//            'h_h':h_h,
//            'h_c':h_c_2phase,
//            'q_flux':q_flux,
//            'Q_wimposed':Q
//        }
//        return dict(Inputs.items()+Outputs.items())
//    
//    
//    def _TwoPhaseH_OnePhaseC_Qimposed(self,Inputs):
//        """
//        Hot stream is condensing, cold stream is single phase  
//        """
//        #Choose which correlation to use.  Shah seems to work better with steam but underpredicts heat transfer for refrigerant
//        #It's possible that Shah only works better for steam because it underpredicts the heat transfer coefficient and the HX has more than enough area for maximum heat transfer
//        if 'Water' or 'water' in self.Ref_h:
//            G_h=self.mdot_h/self.A_h_flow
//            h_h_2phase=ShahCondensation_Average(Inputs['xout_h'],Inputs['xin_h'],self.Ref_h,G_h,self.Dh_h,Inputs['pin_h'],self.Tbubble_h,self.Tdew_h)
//        else:
//            h_h_2phase=LongoCondensation((Inputs['xout_h']+Inputs['xin_h'])/2,self.mdot_h/self.A_h_flow,self.Dh_h,self.Ref_h,self.Tbubble_h,self.Tdew_h)
//        
//        h_c,cp_c,PlateOutput_c=self.PlateHTDP(self.Ref_c, Inputs['Tmean_c'], Inputs['pin_c'],self.mdot_c/self.NgapsCold)
//        #Use cp calculated from delta h/delta T
//        cp_c=Inputs['cp_c']
//        UA_total=1/(1/(h_c*self.A_c_wetted)+1/(h_h_2phase*self.A_h_wetted)+self.PlateThickness/(self.PlateConductivity*(self.A_c_wetted+self.A_h_wetted)/2.))
//        C_c=cp_c*self.mdot_c
//        
//        Q=Inputs['Q']
//        Qmax=C_c*(Inputs['Tsat_h']-Inputs['Tin_c'])
//        epsilon = Q/Qmax
//        
//        if 1 <= epsilon < 1+1e-6:  #if epsilon is slightly larger than 1
//            epsilon = 1-1e-12
//        
//        #Cr = 0, so NTU is simply
//        try:
//            NTU=-log(1-epsilon)
//        except:
//            #pass
//            print 'epsilon_two_phase =', epsilon
//        UA_req=NTU*C_c
//        w=UA_req/UA_total
//        
//        rho_c=Props('D','T',Inputs['Tmean_c'], 'P', self.pin_c, self.Ref_c)
//        Charge_c = w * self.V_c * rho_c
//        rho_h=TwoPhaseDensity(self.Ref_h,Inputs['xout_h'],Inputs['xin_h'],self.Tdew_h,self.Tbubble_h,slipModel='Zivi')
//        Charge_h = w * self.V_h * rho_h
//        
//        #Use Lockhart Martinelli to calculate the pressure drop.  Claesson found good agreement using C parameter of 4.67
//        DP_frict_h=LMPressureGradientAvg(Inputs['xin_h'],Inputs['xout_h'],self.Ref_h,self.mdot_h/self.A_h_flow,self.Dh_h,self.Tbubble_h,self.Tdew_h,C=4.67)*w*self.Lp
//        #Accelerational pressure drop component    
//        DP_accel_h=-AccelPressureDrop(Inputs['xin_h'],Inputs['xout_h'],self.Ref_h,self.mdot_h/self.A_h_flow,self.Tbubble_h,self.Tdew_h)
//        
//        #Pack outputs
//        Outputs={
//            'w': w,
//            'Tout_c': Inputs['Tin_c']+Q/(self.mdot_c*cp_c),
//            'Tout_h': Inputs['Tsat_h'],
//            'DP_c': -PlateOutput_c['DELTAP'],
//            'DP_h': DP_frict_h+DP_frict_h,
//            'Charge_c':Charge_c,
//            'Charge_h':Charge_h,
//            'h_h':h_h_2phase,
//            'h_c':h_c,
//        }
//        
//        return dict(Inputs.items()+Outputs.items())
//    
//    def _TwoPhaseH_OnePhaseC_wimposed(self,Inputs):
//        """
//        Hot stream is condensing, cold stream is single phase  
//        """
//        w = Inputs['w']
//        #Choose which correlation to use.  Shah seems to work better with steam but underpredicts heat transfer for refrigerant
//        #It's possible that Shah only works better for steam because it underpredicts the heat transfer coefficient and the HX has more than enough area for maximum heat transfer
//        if 'Water' or 'water' in self.Ref_h:
//            G_h=self.mdot_h/self.A_h_flow
//            h_h_2phase=ShahCondensation_Average(Inputs['xout_h'],Inputs['xin_h'],self.Ref_h,G_h,self.Dh_h,Inputs['pin_h'],self.Tbubble_h,self.Tdew_h)
//        else:
//            h_h_2phase=LongoCondensation((Inputs['xout_h']+Inputs['xin_h'])/2,self.mdot_h/self.A_h_flow,self.Dh_h,self.Ref_h,self.Tbubble_h,self.Tdew_h)
//        
//        h_c,cp_c,PlateOutput_c=self.PlateHTDP(self.Ref_c, Inputs['Tmean_c'], Inputs['pin_c'],self.mdot_c/self.NgapsCold)
//        #Use cp calculated from delta h/delta T
//        cp_c=Inputs['cp_c']
//        UA_total=1/(1/(h_c*self.A_c_wetted)+1/(h_h_2phase*self.A_h_wetted)+self.PlateThickness/(self.PlateConductivity*(self.A_c_wetted+self.A_h_wetted)/2.))
//        UA_actual = UA_total*w
//        
//        C_c=cp_c*self.mdot_c
//        NTU = UA_actual/C_c
//        epsilon = 1 - exp(-NTU)
//        
//        Qmax=C_c*(Inputs['Tsat_h']-Inputs['Tin_c'])
//        Q = Qmax*epsilon
//        
//        rho_c=Props('D','T',Inputs['Tmean_c'], 'P', self.pin_c, self.Ref_c)
//        Charge_c = w * self.V_c * rho_c
//        rho_h=TwoPhaseDensity(self.Ref_h,Inputs['xout_h'],Inputs['xin_h'],self.Tdew_h,self.Tbubble_h,slipModel='Zivi')
//        Charge_h = w * self.V_h * rho_h
//        
//        #Use Lockhart Martinelli to calculate the pressure drop.  Claesson found good agreement using C parameter of 4.67
//        DP_frict_h=LMPressureGradientAvg(Inputs['xin_h'],Inputs['xout_h'],self.Ref_h,self.mdot_h/self.A_h_flow,self.Dh_h,self.Tbubble_h,self.Tdew_h,C=4.67)*w*self.Lp
//        #Accelerational pressure drop component    
//        DP_accel_h=-AccelPressureDrop(Inputs['xin_h'],Inputs['xout_h'],self.Ref_h,self.mdot_h/self.A_h_flow,self.Tbubble_h,self.Tdew_h)
//        
//        #Pack outputs
//        Outputs={
//            'Tout_c': Inputs['Tin_c']+Q/(self.mdot_c*cp_c),
//            'Tout_h': Inputs['Tsat_h'],
//            'DP_c': -PlateOutput_c['DELTAP'],
//            'DP_h': DP_frict_h+DP_frict_h,
//            'Charge_c':Charge_c,
//            'Charge_h':Charge_h,
//            'h_h':h_h_2phase,
//            'h_c':h_c,
//            'Q_wimposed':Q
//        }
//        
//        return dict(Inputs.items()+Outputs.items())
//    
//    def _TwoPhaseH_TwoPhaseC_Qimposed(self,Inputs):
//        """
//        Hot stream is condensing, cold stream is evaporating 
//        """
//        #Hot side: Shah correlation for steam condensation
//        G_h=self.mdot_h/self.A_h_flow
//        h_h_2phase=ShahCondensation_Average(Inputs['xout_h'],Inputs['xin_h'],self.Ref_h,G_h,self.Dh_h,Inputs['pin_h'],self.Tbubble_h,self.Tdew_h)
//        #Cold side: Cooper Pool Boiling Correlation
//        #Mole mass of refrigerant for Cooper correlation
//        M=Props('M','T',0,'P',0,self.Ref_c)
//        #Reduced pressure for Cooper Correlation
//        pstar=Inputs['pin_c']/Props('E','T',0,'P',0,self.Ref_c)
//          
//        """
//    The Cooper Pool boiling relationship is a function of the heat flux, which is known in this particular case, but w
//    has to be determined by iteration
//    """
//        change=999
//        w=1
//        while abs(change)>1e-6:
//            q_flux=Inputs['Q']/(w*self.A_c_wetted)
//            #Heat transfer coefficient from Cooper Pool Boiling
//            h_c_2phase=Cooper_PoolBoiling(pstar,1.0,q_flux,M) #1.5 correction factor comes from Claesson Thesis on plate HX
//   
//            G=self.mdot_c/self.A_c_flow
//            Dh=self.Dh_c
//            x=(Inputs['xin_c']+Inputs['xout_c'])/2
//            DELTAT=Inputs['Tsat_h']-Inputs['Tsat_c']
//            UA_req=Inputs['Q']/DELTAT
//            UA_total=1/(1/(h_h_2phase*self.A_h_wetted)+1/(h_c_2phase*self.A_c_wetted)+self.PlateThickness/(self.PlateConductivity*self.A_c_wetted))
//            change=UA_req/UA_total-w
//            w=UA_req/UA_total
//        #print UA_req,DELTAT
//        #Refrigerant charge
//        rho_h=TwoPhaseDensity(self.Ref_h,Inputs['xin_h'],Inputs['xout_h'],self.Tdew_h,self.Tbubble_h,slipModel='Zivi')
//        Charge_h = w * self.V_h * rho_h
//        rho_c=TwoPhaseDensity(self.Ref_c,Inputs['xin_c'],Inputs['xout_c'],self.Tdew_c,self.Tbubble_c,slipModel='Zivi')
//        Charge_c = rho_c * w * self.V_c
//     
//        #Use Lockhart Martinelli to calculate the pressure drop.  Claesson found good agreement using C parameter of 4.67
//        DP_frict_c=LMPressureGradientAvg(Inputs['xin_c'],Inputs['xout_c'],self.Ref_c,self.mdot_c/self.A_c_flow,self.Dh_c,self.Tbubble_c,self.Tdew_c,C=4.67)*w*self.Lp
//        #Accelerational pressure drop component    
//        DP_accel_c=AccelPressureDrop(Inputs['xin_c'],Inputs['xout_c'],self.Ref_c,self.mdot_c/self.A_c_flow,self.Tbubble_c,self.Tdew_c)
//        #Use Lockhart Martinelli to calculate the pressure drop.  Claesson found good agreement using C parameter of 4.67
//        DP_frict_h=LMPressureGradientAvg(Inputs['xin_h'],Inputs['xout_h'],self.Ref_h,self.mdot_h/self.A_h_flow,self.Dh_h,self.Tbubble_h,self.Tdew_h,C=4.67)*w*self.Lp
//        #Accelerational pressure drop component    
//        DP_accel_h=AccelPressureDrop(Inputs['xin_h'],Inputs['xout_h'],self.Ref_h,self.mdot_h/self.A_h_flow,self.Tbubble_h,self.Tdew_h)
//    
//        #Pack outputs
//        Outputs={
//            'w':w,
//            'Charge_c': Charge_c,
//            'Charge_h': Charge_h,
//            'Tout_c': Inputs['Tsat_c'],
//            'Tout_h': Inputs['Tsat_h'],
//            'DP_h': DP_frict_h+DP_accel_h,
//            'DP_c': DP_frict_c+DP_accel_c,
//            'h_h':h_h_2phase,
//            'h_c':h_c_2phase,
//            'q_flux':q_flux
//        }
//        return dict(Inputs.items()+Outputs.items())
//        
//    def Calculate(self):
//        """
//        Calculate the PHE
//        
//        """
//        
//        # Allocate channels between hot and cold streams
//        if not hasattr(self,'MoreChannels') or self.MoreChannels not in ['Hot','Cold']:
//            raise KeyError("MoreChannels not found, options are 'Hot' or 'Cold'")
//        #There are (Nplates - 1) gaps between the plates
//        if self.MoreChannels=='Hot':
//            #Hot stream gets the extra channel
//            self.NgapsHot=(self.Nplates-1)//2+1
//            self.NgapsCold=self.Nplates-1-self.NgapsHot
//        else:
//            #Cold stream gets the extra channel
//            self.NgapsCold=(self.Nplates-1)//2+1
//            self.NgapsHot=self.Nplates-1-self.NgapsCold
//        
//        #Saturation temperatures for cold fluid
//        self.Tbubble_c=Tsat(self.Ref_c,self.pin_c,0,0)
//        self.Tdew_c=Tsat(self.Ref_c,self.pin_c,1,0)
//        self.Tsat_c=(self.Tbubble_c+self.Tdew_c)/2.0
//        if IsFluidType(self.Ref_c,'Brine'):
//            self.rhosatL_c=1
//            self.rhosatV_c=1
//        else:
//            self.rhosatL_c=Props('D','T',self.Tbubble_c,'Q',0,self.Ref_c)
//            self.rhosatV_c=Props('D','T',self.Tdew_c,'Q',1,self.Ref_c)
//        
//        #Saturation temperatures for hot fluid
//        self.Tbubble_h=Tsat(self.Ref_h,self.pin_h,0,0)
//        self.Tdew_h=Tsat(self.Ref_h,self.pin_h,1,0)
//        self.Tsat_h=(self.Tbubble_h+self.Tdew_h)/2.0
//        if IsFluidType(self.Ref_h,'Brine'):
//            self.rhosatL_h=1
//            self.rhosatV_h=1
//        else:
//            self.rhosatL_h=Props('D','T',self.Tbubble_h,'Q',0,self.Ref_h)
//            self.rhosatV_h=Props('D','T',self.Tdew_h,'Q',1,self.Ref_h)
//        
//        #The rest of the inlet states
//        self.Tin_h,self.rhoin_h=TrhoPhase_ph(self.Ref_h,self.pin_h,self.hin_h,self.Tbubble_h,self.Tdew_h,self.rhosatL_h,self.rhosatV_h)[0:2]
//        self.Tin_c,self.rhoin_c=TrhoPhase_ph(self.Ref_c,self.pin_c,self.hin_c,self.Tbubble_c,self.Tdew_c,self.rhosatL_c,self.rhosatV_c)[0:2]
//        
//        if IsFluidType(self.Ref_c,'Brine'):
//            self.sin_c=Props('S','T',self.Tin_c,'P',self.pin_c,self.Ref_c)*1000
//        else:
//            self.sin_c=Props('S','T',self.Tin_c,'D',self.rhoin_c,self.Ref_c)*1000
//            
//        if IsFluidType(self.Ref_h,'Brine'):
//            self.sin_h=Props('S','T',self.Tin_h,'P',self.pin_h,self.Ref_h)*1000
//        else:
//            self.sin_h=Props('S','T',self.Tin_h,'D',self.rhoin_h,self.Ref_h)*1000
//            
//        # Find HT and Delta P on the hot side
//        #---------------
//        #Mean values for the hot side based on average of inlet temperatures
//        HotPlateInputs={
//            'PlateAmplitude': self.PlateAmplitude,
//            'PlateWavelength' : self.PlateWavelength,
//            'InclinationAngle': self.InclinationAngle,
//            'Bp': self.Bp,
//            'Lp': self.Lp
//        }
//        HotPlateOutputs=PHE_1phase_hdP(HotPlateInputs,JustGeo=True)
//        #There are (Nplates-2) active plates (outer ones don't do anything)
//        self.A_h_wetted=HotPlateOutputs['Ap']*(self.Nplates-2)
//        self.V_h=HotPlateOutputs['Vchannel']*self.NgapsHot
//        self.A_h_flow=HotPlateOutputs['Aflow']*self.NgapsHot
//        self.Dh_h=HotPlateOutputs['Dh']
//        
//        # Find geometric parameters for cold side of plates
//        ColdPlateInputs={
//            'PlateAmplitude': self.PlateAmplitude,
//            'PlateWavelength' : self.PlateWavelength,
//            'InclinationAngle': self.InclinationAngle,
//            'Bp': self.Bp,
//            'Lp': self.Lp
//        }
//        ColdPlateOutputs=PHE_1phase_hdP(ColdPlateInputs,JustGeo=True)
//        #There are (Nplates-2) active plates (outer ones don't do anything)
//        self.A_c_wetted=ColdPlateOutputs['Ap']*(self.Nplates-2)
//        self.V_c=ColdPlateOutputs['Vchannel']*self.NgapsCold
//        self.A_c_flow=ColdPlateOutputs['Aflow']*self.NgapsCold
//        self.Dh_c=HotPlateOutputs['Dh']
//        
//        #Figure out the limiting rate of heat transfer
//        self.Qmax=self.DetermineHTBounds()
//        
//        def GivenQ(Q):
//            """
//            In this function, the heat transfer rate is imposed.  Therefore the
//            outlet states for both fluids are known, and each element can be solved
//            analytically in one shot without any iteration.
//            """
//            
//            EnthalpyList_c,EnthalpyList_h=self.BuildEnthalpyLists(Q)
//                
//#            #Plot temperature v. h profiles
//#            for i in range(len(EnthalpyList_c)-1):
//#                hc=np.linspace(EnthalpyList_c[i],EnthalpyList_c[i+1])
//#                Tc=np.zeros_like(hc)
//#                for j in range(len(hc)):
//#                    Tc[j],r,Ph=TrhoPhase_ph(self.Ref_c,self.pin_c,hc[j],self.Tbubble_c,self.Tdew_c,self.rhosatL_c,self.rhosatV_c)
//#                pylab.plot(self.mdot_c*(hc-EnthalpyList_c[0])/1000,Tc,'b')
//#                
//#            for i in range(len(EnthalpyList_h)-1):
//#                hh=np.linspace(EnthalpyList_h[i],EnthalpyList_h[i+1])
//#                Th=np.zeros_like(hh)
//#                for j in range(len(hh)):
//#                    Th[j],r,Ph=TrhoPhase_ph(self.Ref_h,self.pin_h,hh[j],self.Tbubble_h,self.Tdew_h,self.rhosatL_h,self.rhosatV_h)
//#                pylab.plot(self.mdot_h*(hh-EnthalpyList_h[0])/1000,Th,'r')
//#            pylab.show()
//#                
//#            Ph(self.Ref_h)
//#            pylab.plot(np.array(EnthalpyList_h)/1000,self.pin_h*np.ones_like(EnthalpyList_h))
//#            pylab.show()
//            
//            I_h=0
//            I_c=0
//            wList=[]
//            cellList=[]
//            while I_h<len(EnthalpyList_h)-1:
//                #Heat Transfer Occuring in Each Cell
//                #We already built an enthalpy list above that balances energy so we can use either the hot or cold side to find the heat transfer rate in the cell    
//                Qbound_h=self.mdot_h*(EnthalpyList_h[I_h+1]-EnthalpyList_h[I_h])
//                Qbound=Qbound_h
//                
//                #Figure out the inlet and outlet enthalpy for this cell
//                hout_h=EnthalpyList_h[I_h]
//                hin_h=EnthalpyList_h[I_h+1]
//                hin_c=EnthalpyList_c[I_c]
//                hout_c=EnthalpyList_c[I_c+1]
//                
//                # Figure out what combination of phases you have:
//                # -------------------------------------------------
//                # Hot stream is either single phase or condensing
//                # Cold stream is either single phase or evaporating
//                
//                #Use midpoint enthalpies to figure out the phase in the cell
//                Phase_h=Phase_ph(self.Ref_h,self.pin_h,(hin_h+hout_h)/2,self.Tbubble_h,self.Tdew_h,self.rhosatL_h,self.rhosatV_h)
//                Phase_c=Phase_ph(self.Ref_c,self.pin_c,(hin_c+hout_c)/2,self.Tbubble_c,self.Tdew_c,self.rhosatL_c,self.rhosatV_c)
//                #Determine inlet and outlet temperatures to the cell ([0] gives the first element of the tuple which is temeperature)
//                Tin_h=TrhoPhase_ph(self.Ref_h,self.pin_h,hin_h,self.Tbubble_h,self.Tdew_h,self.rhosatL_h,self.rhosatV_h)[0]
//                Tin_c=TrhoPhase_ph(self.Ref_c,self.pin_c,hin_c,self.Tbubble_c,self.Tdew_c,self.rhosatL_c,self.rhosatV_c)[0]
//                Tout_h=TrhoPhase_ph(self.Ref_h,self.pin_h,hout_h,self.Tbubble_h,self.Tdew_h,self.rhosatL_h,self.rhosatV_h)[0]
//                Tout_c=TrhoPhase_ph(self.Ref_c,self.pin_c,hout_c,self.Tbubble_c,self.Tdew_c,self.rhosatL_c,self.rhosatV_c)[0]
//                
//                if Phase_h in ['Subcooled','Superheated'] and Phase_c in ['Subcooled','Superheated']:
//                    # Both are single-phase
//                    Inputs={
//                        'Q':Qbound,
//                        'cp_h':(hin_h-hout_h)/(Tin_h-Tout_h),
//                        'cp_c':(hin_c-hout_c)/(Tin_c-Tout_c),
//                        'Tmean_h':(Tin_h+Tout_h)/2,
//                        'Tmean_c':(Tin_c+Tout_c)/2,
//                        'Tin_h':Tin_h,
//                        'Tin_c':Tin_c,
//                        'pin_h':self.pin_h,
//                        'pin_c':self.pin_c,
//                        'Phase_c':Phase_c,
//                        'Phase_h':Phase_h
//                    }
//                    Outputs=self._OnePhaseH_OnePhaseC_Qimposed(Inputs)
//                    Outputs['Tout_c_balance'] = Tout_c
//                    Outputs['Tout_h_balance'] = Tout_h
//                    wList.append(Outputs['w'])
//                    cellList.append(Outputs)
//                    if self.Verbosity>6:
//                        print 'w[1-1]: ', Outputs['w']
//                elif Phase_h=='TwoPhase' and Phase_c in ['Subcooled','Superheated']:
//                    # Hot stream is condensing, and cold stream is single-phase (SH or SC)
//                    # TODO: bounding state can be saturated state if hot stream is condensing
//                    #Must be two-phase so quality is defined
//                    xin_h=(hin_h-self.hsatL_h)/(self.hsatV_h-self.hsatL_h)
//                    xout_h=(hout_h-self.hsatL_h)/(self.hsatV_h-self.hsatL_h)
//                    Inputs={
//                        'Q':Qbound,
//                        'xin_h':xin_h,
//                        'xout_h':xout_h,
//                        'Tsat_h':self.Tsat_h,
//                        'Tmean_c':(Tin_c+Tout_c)/2,
//                        'cp_c':(hin_c-hout_c)/(Tin_c-Tout_c),
//                        'Tin_h':Tin_h,
//                        'Tin_c':Tin_c,
//                        'pin_h':self.pin_h,
//                        'pin_c':self.pin_c,
//                        'Phase_c':Phase_c,
//                        'Phase_h':Phase_h
//                    }
//                    Outputs=self._TwoPhaseH_OnePhaseC_Qimposed(Inputs)
//                    Outputs['Tout_c_balance'] = Tout_c
//                    Outputs['Tout_h_balance'] = Tout_h
//                    if self.Verbosity>6:
//                        print 'w[2-1]: ', Outputs['w']
//                    wList.append(Outputs['w'])
//                    cellList.append(Outputs)
//                elif Phase_c=='TwoPhase' and Phase_h in ['Subcooled','Superheated']:
//                    # Cold stream is evaporating, and hot stream is single-phase (SH or SC)
//                    
//                    #Must be two-phase so quality is defined
//                    xin_c=(hin_c-self.hsatL_c)/(self.hsatV_c-self.hsatL_c)
//                    xout_c=(hout_c-self.hsatL_c)/(self.hsatV_c-self.hsatL_c)
//                    
//                    Inputs={
//                        'Q':Qbound,
//                        'xin_c':xin_c,
//                        'xout_c':xout_c,
//                        'Tsat_c':self.Tsat_c,
//                        'cp_h':(hin_h-hout_h)/(Tin_h-Tout_h),
//                        'Tmean_h':(Tin_h+Tout_h)/2,
//                        'Tin_h':Tin_h,
//                        'Tin_c':Tin_c,
//                        'pin_h':self.pin_h,
//                        'pin_c':self.pin_c,
//                        'Phase_c':Phase_c,
//                        'Phase_h':Phase_h
//                    }
//                    Outputs=self._OnePhaseH_TwoPhaseC_Qimposed(Inputs)
//                    Outputs['Tout_c_balance'] = Tout_c
//                    Outputs['Tout_h_balance'] = Tout_h
//                    if self.Verbosity>6:
//                        print 'w[1-2]: ', Outputs['w']
//                    wList.append(Outputs['w'])
//                    cellList.append(Outputs)
//                    
//                elif Phase_c=='TwoPhase' and Phase_h=='TwoPhase':
//                    # Cold stream is evaporating, and hot stream is condensing
//                    
//                    #Must be two-phase so quality is defined
//                    xin_c=(hin_c-self.hsatL_c)/(self.hsatV_c-self.hsatL_c)
//                    xout_c=(hout_c-self.hsatL_c)/(self.hsatV_c-self.hsatL_c)
//                    xin_h=(hin_h-self.hsatL_h)/(self.hsatV_h-self.hsatL_h)
//                    xout_h=(hout_h-self.hsatL_h)/(self.hsatV_h-self.hsatL_h)
//                    
//                    Inputs={
//                        'Q':Qbound,
//                        'xin_c':xin_c,
//                        'xout_c':xout_c,
//                        'xin_h':xin_h,
//                        'xout_h':xout_h,
//                        'Tsat_c':self.Tsat_c,
//                        'Tsat_h':self.Tsat_h,
//                        'Tin_h':Tin_h,
//                        'Tin_c':Tin_c,
//                        'pin_h':self.pin_h,
//                        'pin_c':self.pin_c,
//                        'Phase_c':Phase_c,
//                        'Phase_h':Phase_h
//                    }
//                    Outputs=self._TwoPhaseH_TwoPhaseC_Qimposed(Inputs)
//                    Outputs['Tout_c_balance'] = Tout_c
//                    Outputs['Tout_h_balance'] = Tout_h
//                    if self.Verbosity>6:
//                        print 'w[2-2]: ', Outputs['w']
//                    wList.append(Outputs['w'])
//                    cellList.append(Outputs)
//                    
//                I_h+=1
//                I_c+=1
//            #end while loop
//            
//            self.cellList=cellList
//            if self.Verbosity>6:
//                print 'wsum:', np.sum(wList)
//            return np.sum(wList)-1.0
//        try:
//            brentq(GivenQ,0.01*self.Qmax,self.Qmax)#,xtol=0.000001*self.Qmax)
//        except ValueError as e:
//            if e.args[0]=='f(a) and f(b) must have different signs':
//                #if we get this error, we assume Qmax is actually achieved.  It means there was more than enough
//                #area to reach Qmax so np.sum(wList)-1.0 above is still negative even when Qmax is the input  
//                print "brentq Exception Occurred!!!"
//                GivenQ(self.Qmax)
//                
//                #check which end of the HX has a DELTA_T of 0
//                if self.cellList[0]['Tin_c']-1e-6 < self.cellList[0]['Tout_h'] < self.cellList[0]['Tin_c']+1e-6:  #if outlet of hot stream is equal to inlet of cold stream within a tolerance
//                    longcell = 0
//                elif self.cellList[-1]['Tin_h']-1e-6 < self.cellList[-1]['Tout_c'] < self.cellList[-1]['Tin_h']+1e-6:
//                    longcell = len(self.cellList)-1
//                else:
//                    raise ValueError('We have a problem. Neither end cell has a DELTA_T of 0!')
//                #end if
//                
//                #find the length fraction of the long cell
//                w_totalused = 0
//                for dictio in self.cellList:
//                    w_totalused += dictio['w']
//                #end for
//                w_leftover = 1 - w_totalused  #this is the length fraction that was not needed to get maximum heat transfer
//                
//                self.cellList[longcell]['w'] += w_leftover  #add the unneeded length fraction to the required length fraction for the long cell
//                                
//                #do something depending on the phases we have in that cell
//
//                if self.cellList[longcell]['Phase_h'] in ['Subcooled','Superheated'] \
//                and self.cellList[longcell]['Phase_c'] in ['Subcooled','Superheated']:  #then we have single phase flow only
//                    #overwrite the cell with the outputs we get from imposing the oversized length fraction on the cell
//                    self.cellList[longcell] = self._OnePhaseH_OnePhaseC_wimposed(self.cellList[longcell])
//                    
//                elif self.cellList[longcell]['Phase_h']=='TwoPhase' \
//                and self.cellList[longcell]['Phase_c'] in ['Subcooled','Superheated']:
//                    
//                    self.cellList[longcell] = self._TwoPhaseH_OnePhaseC_wimposed(self.cellList[longcell])
//                    
//                elif self.cellList[longcell]['Phase_h'] in ['Subcooled','Superheated'] \
//                and self.cellList[longcell]['Phase_c']=='TwoPhase':
//                    
//                    self.cellList[longcell] = self._OnePhaseH_TwoPhaseC_wimposed(self.cellList[longcell])
//                else:
//                    raise ValueError('Phases in long cell are not valid')
//                
//            else:
//                raise  #re-raise the exception if it is a different ValueError than the one above
//            #end if
//        # Collect parameters from all the pieces
//        self.PostProcess(self.cellList)
//    #end Calculate()
//        
void BrazedPlateHeatExchanger::test()
{
	// R134a evaporator, powered with hot water
	this->State_c_inlet = CoolPropStateClass("Propane");
	this->State_c_inlet.update(iT,280,iQ,0.3);
	this->mdot_c = 0.030;
	
	this->State_h_inlet = CoolPropStateClass("Nitrogen");
	this->State_h_inlet.update(iT,600,iP,101.325);
	this->mdot_h = 0.30;
}
//def Evaporator_Datapoints():
//        
//    Tin_h_list=[115.6641168]
//    mdot_c_list=[1]
//    Tin_c_list=[40]
//    pin_c_list=[4000]
//    mdot_h_list=[1]
//
//#    Tin_h_list=[115.6641168]  #Superheated vapor cold, superheated steam hot
//#    mdot_c_list=[0.2]
//#    Tin_c_list=[50]
//#    pin_c_list=[1000]
//#    mdot_h_list=[0.01]
//    
//    
//#    Tin_h_list=[109.2032074,109.1851234,108.4549482,107.9791489,107.6165509,107.4101384,107.2647048,104.1383582,104.3328478,104.5771679,104.9493369,101.5880664,101.8178977,99.20625225,99.90661879,101.3846584,105.2110387,105.1862496,115.5595054,115.6003708,115.6641168,115.7297965,115.9511292,115.6199168,105.0232727,105.5598757,106.5037243,101.6031291,101.9670669,102.4259946,103.3071928,103.958456,105.3671805,108.4542309,108.45119,107.7000694,101.7041459,100.6150463,97.83929338,99.63130551,100.7364537,101.0184892,101.3360446,98.12927191,98.79647217,103.5567338,104.8639693,109.4783441,114.9712029,117.0526462,117.1202029,114.7134448,97.05882883,96.94468476]
//#    mdot_c_list=[0.134822695,0.13572093,0.137639286,0.138838652,0.139621053,0.140007005,0.140300537,0.165602837,0.165053097,0.16465,0.163166667,0.19159246,0.190964727,0.214956757,0.21397695,0.224382562,0.162521968,0.162625659,0.072996429,0.072871658,0.072578761,0.072375221,0.071458407,0.072870796,0.163247772,0.162628885,0.159740541,0.191456364,0.190269439,0.188806104,0.186052252,0.182439856,0.213030686,0.209687273,0.207797491,0.206937722,0.203731317,0.223618538,0.222488372,0.220371226,0.214896797,0.212716606,0.2099625,0.222661896,0.213822262,0.182906475,0.162743682,0.133810036,0.072262681,0.043415162,0.042835145,0.072994662,0.224468468,0.224882662]    
//#    Tin_c_list=[22.03715426,23.16476565,21.56729107,20.59805674,19.78935965,19.29071804,19.02032737,20.3202234,20.41146018,20.51750714,20.70542553,21.52692998,21.81500353,23.16804865,23.49247518,24.59599466,20.61169596,20.57335501,16.50406964,16.35204635,16.34639823,16.5091646,16.7419292,16.22760531,20.98519786,21.96719927,24.38090631,21.98184545,22.8712821,23.89128546,25.80212793,26.49839677,25.32141877,27.82343818,28.24085305,27.50246085,27.58272954,24.63232264,25.30931306,26.9629325,27.414121,29.16145848,29.21683214,26.14887299,23.72624955,25.90484892,21.20190072,22.81844265,17.57384783,23.53998736,27.03147645,17.3677331,25.12729189,25.18876182]        
//#    pin_c_list=[2647.535555,2346.886987,1919.108407,1646.507683,1463.059179,1327.4258,1230.359492,1406.757775,1521.740582,1667.224268,1854.224582,1563.900865,1688.189146,1597.203816,1704.831254,1665.19234,1841.721889,1848.438996,952.4051857,1083.191565,1294.087865,1418.889703,1927.217908,1131.01168,1799.738278,2075.009766,2480.086679,1670.415902,1820.935165,2017.784921,2287.786353,2436.77021,1839.338818,2003.437716,2208.164102,2303.190842,2347.948868,1744.506804,1855.218996,2018.876339,2196.113854,2294.72928,2391.850691,1862.269891,1574.800479,2324.151836,1839.64509,2616.118523,1381.744736,700.1896625,908.1878786,840.5895053,1555.263688,1487.360375]
//#    mdot_h_list=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.024367472,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.023536256,0.022650033,0.022747971,0.022850915,0,0.024482118,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.025126404,0.025004233]
//    i = 0
//
//    for Tin_h, mdot_c, Tin_c, pin_c, mdot_h in zip(Tin_h_list, mdot_c_list, Tin_c_list, pin_c_list, mdot_h_list):
//        
//        pin_h = Props('P','T',Tin_h+273.15,'Q',1,'Water.fld')
//        
//        params={
//                'Ref_c':'R134a.fld',
//                'mdot_c':mdot_c,
//                'pin_c':pin_c,
//                'hin_c':Props('H','T',Tin_c+273.15,'P',pin_c,'R134a.fld')*1000,
//                #'xin_h':0.0,
//        
//                'Ref_h':'Water.fld',
//                'mdot_h':mdot_h,
//                'pin_h':pin_h,
//                'hin_h':Props('H','T',Tin_h+273.15,'Q',1,'Water.fld')*1000,#Props('H','T',Tin_h+273.15,'Q',1.0,'Water.fld')*1000,
//        
//        
//        #Geometric parameters
//        'Bp' : 0.119,
//        'Lp' : 0.526, #Center-to-center distance between ports
//        'Nplates' : 110,
//        'PlateAmplitude' : 0.00102, #[m]
//        'PlateThickness' : 0.0003, #[m]
//        'PlateWavelength' : 0.0066, #[m]
//        'InclinationAngle' : pi/3,#[rad]
//        'PlateConductivity' : 15.0, #[W/m-K]
//        'MoreChannels' : 'Hot', #Which stream gets the extra channel, 'Hot' or 'Cold'
//    
//        'Verbosity':6
//        }
//        PHE=PHEHXClass(**params)
//        PHE.Calculate()
//        print PHE.OutputList()
//        #        Write2CSV(PHE,'Evaporator_testing.csv',append=i>0)
//        wlist = [0]
//        Tc_list = [PHE.cellList[0]['Tin_c']]
//        Tc_balance_list = [PHE.cellList[0]['Tin_c']]
//        
//        Th_list = [PHE.cellList[0]['Tout_h']]
//        Th_balance_list = [PHE.cellList[0]['Tout_h_balance']]
//        for dictio in PHE.cellList:
//            Tc_list.append(dictio['Tout_c'])
//            Tc_balance_list.append(dictio['Tout_c_balance'])
//            
//            Th_list.append(dictio['Tin_h'])
//            Th_balance_list.append(dictio['Tin_h'])
//            
//            wlist.append(wlist[-1] + dictio['w'])
//            print dictio
//        print wlist
//        print Tc_list
//        print Tc_balance_list
//        print
//        print Th_list
//        print Th_balance_list
//        
//        figure()
//        plot(wlist, Tc_list, label=str.split(PHE.Ref_c,'.')[0])
//        plot(wlist, Th_list, label=str.split(PHE.Ref_h,'.')[0])
//        xlim(0,1)
//        xlabel('HX Length Fraction [-]')
//        ylabel('Fluid Temperature [K]')
//        leg=legend(loc='best', fancybox=True)
//        leg.get_frame().set_alpha(0.5)
//        
//        show()
//        
//        i += 1
//    #end for
//    #show()
//
//def Condenser_Datapoints():
//        
//    #Tdew=Tsat('R134a.fld',962.833,1.0,0)
//    Tin_c_list=[14.5252021276596]
//    Tin_h_list=[69.0936205673759]
//    pin_h_list=[655.7412447]
//    pin_c_list=[413.986484]
//#    mdot_c_list=[0.637700355]
//    mdot_c_list=[0.02]
//    mdot_h_list=[0.134822695]
//    
//    
//#    Tin_c_list=[14.5252021276596,18.0372343470483,17.4613803571429,16.9616932624113,16.5561368421053,16.2787408056042,16.0128550983900,15.8115567375887,15.6998690265487,15.5883750000000,15.5138439716312,15.2766768402155,15.2238447971781,15.1646306306306,15.1466223404255,15.0051423487545,14.8197768014060,14.8186362038664,14.8433446428571,14.4890695187166,14.4478053097345,14.4545238938053,14.3051274336283,14.2819876106195,15.5974260249555,15.2699524680073,15.0345621621622,14.9595963636364,14.9513110307414,14.8921059245961,14.8095423423423,14.8001346499102,14.8710559566787,14.9003109090909,14.9301236559140,13.9689341637011,13.8059341637011,13.4422976827095,13.6025635062612,13.6897566607460,13.7342793594306,15.2341119133574,15.1533214285714,15.1328765652952,15.1223141831239,15.1155881294964,15.1335776173285,15.1193476702509,14.8900398550725,14.8169404332130,14.8503967391304,14.9210871886121,14.8191171171171,14.8690210157618]
//#    Tin_h_list=[69.0936205673759,73.0083094812165,77.3911464285713,81.4898439716307,85.0726000000000,88.6417530647987,92.4448121645795,86.1455585106383,82.591433628320,78.0308660714283,74.3252021276597,80.2230556552966,76.7632398589063,79.7378576576576,76.5262500000001,81.0034412811387,74.8595536028120,74.6893198594024,97.9000678571429,94.0734010695188,89.6514495575222,88.1463079646017,84.9626796460177,92.7013752212390,76.1706007130121,72.5404314442414,68.5904234234235,77.5405709090910,74.2168806509946,70.9132477558349,67.5777621621621,66.6234631956911,79.6618664259930,79.0482272727271,75.5807347670249,72.8854501779359,65.6904163701068,77.5234919786094,71.8477352415026,69.9828436944938,67.1498754448397,66.5621696750902,65.4449071428572,72.7005849731664,80.4468384201075,66.2388830935252,74.5471642599277,69.9765878136200,88.8145054347826,104.896774368231,96.9835579710145,103.755985765124,81.8131009009010,85.8043029772330]
//#    pin_h_list=[655.7412447,738.3980805,734.8051089,730.7268723,726.1262825,721.6028634,719.1346744,757.5090053,756.2224867,733.0675464,725.9573741,766.3976715,761.9741728,793.8355027,787.0453103,808.6531868,719.7610967,720.2668067,583.991525,581.1295116,575.1926177,571.817108,566.3421876,574.943154,726.1784296,717.7605009,705.1538252,771.4394655,763.8691374,758.5246948,745.4522072,739.9428294,809.5384422,797.50388,795.7923566,774.9121032,763.4721566,803.898574,807.1820089,799.3536234,769.0709057,801.7964152,797.8371232,833.8101145,801.4951741,736.1348381,718.0653123,662.2176452,583.5632554,546.6169513,545.2907264,593.2933203,822.3789423,825.2910543]
//#    pin_c_list=[413.986484,394.2711127,395.9132357,393.7606933,391.7209211,392.0065236,386.8062898,385.5954982,380.4885062,434.4664161,438.783516,423.3637433,426.8823086,430.6951694,441.8179645,416.8191477,398.9098084,398.4793023,409.6125643,386.0327237,409.4259009,430.1905398,433.083131,405.4200496,434.6688806,423.0569433,405.8295135,396.4926455,400.6509711,396.4682801,395.4844739,391.4224129,375.9417599,371.6675509,370.6234391,370.9965463,369.384484,368.3856007,360.5570894,358.8223801,392.6586139,378.3434657,374.2692179,373.6235725,423.5517397,434.344268,436.4904819,445.1581756,420.7101431,405.370083,404.5810888,397.3699893,391.6934541,388.6189527]
//#    mdot_c_list=[0.637700355,0.619313059,0.620996429,0.61873227,0.616685965,0.616968476,0.611500894,0.611978723,0.606856637,0.659598214,0.663388298,0.653949731,0.657130511,0.662484685,0.672641844,0.650140569,0.627421793,0.627365554,0.630610714,0.605855615,0.629874336,0.650260177,0.65239469,0.625984071,0.662096257,0.650639854,0.633111712,0.627227273,0.630905967,0.626073609,0.624209009,0.619689408,0.607581227,0.6022,0.600435484,0.599911032,0.597104982,0.599081996,0.589948122,0.587230906,0.622339858,0.609075812,0.604291071,0.605788909,0.65556912,0.662206835,0.663487365,0.66928853,0.64076087,0.621785199,0.620882246,0.617604982,0.626063063,0.623537653]
//#    mdot_h_list=[0.134822695,0.13572093,0.137639286,0.138838652,0.139621053,0.140007005,0.140300537,0.165602837,0.165053097,0.16465,0.163166667,0.19159246,0.190964727,0.214956757,0.21397695,0.224382562,0.162521968,0.162625659,0.072996429,0.072871658,0.072578761,0.072375221,0.071458407,0.072870796,0.163247772,0.162628885,0.159740541,0.191456364,0.190269439,0.188806104,0.186052252,0.182439856,0.213030686,0.209687273,0.207797491,0.206937722,0.203731317,0.223618538,0.222488372,0.220371226,0.214896797,0.212716606,0.2099625,0.222661896,0.213822262,0.182906475,0.162743682,0.133810036,0.072262681,0.043415162,0.042835145,0.072994662,0.224468468,0.224882662]    
//    i = 0
//    zip(Tin_c_list,Tin_h_list,pin_h_list,pin_c_list,mdot_c_list,mdot_h_list)
//    for Tin_c,Tin_h,pin_h,pin_c,mdot_c,mdot_h in zip(Tin_c_list,Tin_h_list,pin_h_list,pin_c_list,mdot_c_list,mdot_h_list):
//        params={
//        'Ref_c':'Water.fld',
//        'mdot_c':mdot_c,
//        'pin_c':pin_c,
//        'hin_c':Props('H','T',Tin_c+273.15,'P',pin_c,'Water.fld')*1000,
//        #'xin_c':0.0,
//        
//        'Ref_h':'R134a.fld',
//        'mdot_h':mdot_h,
//        'pin_h':pin_h,
//        'hin_h':Props('H','T',Tin_h+273.15,'P',pin_h,'R134a.fld')*1000,
//        
//        #Geometric parameters
//        'Bp' : 0.119,
//        'Lp' : 0.526, #Center-to-center distance between ports
//        'Nplates' : 110,
//        'PlateAmplitude' : 0.00102, #[m]
//        'PlateThickness' : 0.0003, #[m]
//        'PlateWavelength' : 0.0066, #[m]
//        'InclinationAngle' : pi/3,#[rad]
//        'PlateConductivity' : 15.0, #[W/m-K]
//        'MoreChannels' : 'Hot', #Which stream gets the extra channel, 'Hot' or 'Cold'
//    
//        'Verbosity':6
//    }
//        PHE=PHEHXClass(**params)
//        PHE.Calculate()
//        print PHE.OutputList()
//#        Write2CSV(PHE,'Condenser_testing.csv',append=i>0)
//        wlist = [0]
//        Tc_list = [PHE.cellList[0]['Tin_c']]
//        Tc_balance_list = [PHE.cellList[0]['Tin_c']]
//        
//        Th_list = [PHE.cellList[0]['Tout_h']]
//        Th_balance_list = [PHE.cellList[0]['Tout_h_balance']]
//        for dictio in PHE.cellList:
//            Tc_list.append(dictio['Tout_c'])
//            Tc_balance_list.append(dictio['Tout_c_balance'])
//            
//            Th_list.append(dictio['Tin_h'])
//            Th_balance_list.append(dictio['Tin_h'])
//            
//            wlist.append(wlist[-1] + dictio['w'])
//            print dictio
//        print wlist
//        print Tc_list
//        print Tc_balance_list
//        print
//        print Th_list
//        print Th_balance_list
//        
//        figure()
//        plot(wlist, Tc_list, wlist, Th_list)
//        xlim(0,1)
//        show()
//        
//        i += 1