/////////////////////////////////////////////////////////////////////////////////
//
//  Test application for Platinum development
//
//  $LastChangedBy$
//

//    The Platinum library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    The Platinum library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with the Platinum library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include "platinumprogram.h"

#include <cstdio>
#include <iostream>

#include "global.h"
#include "image_binary.h"
#include "image_scalar.h"
#include "image_integer.hxx"

//---- FLTK curve testing -----
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Toggle_Button.H>

//----- List testing ------------
#include <set>
#include "points_seq_func1D.h"

//------------------------------------------
//------------------------------------------
//------------------------------------------

template<class S>
class a{
public:
	S data;
	virtual void f(){cout<<"1"<<endl;}
	virtual float g()=0;
};

template<class T>
class b:a<T>{
public:
	void f(){cout<<"b.f"<<endl;}
	float g(){cout<<"b.g"<<endl; return data;}
	//	T h()=0;
};

//------------------------------------------
//------------------------------------------
//------------------------------------------
class MR_signal{

public:
	MR_signal(int nr_tissue_comp);
	int M;					//No of tissue components
	std::vector<double> ro;		//the "densities" of the M different tissues
	std::vector<double> df;		//the "chemical shifts" of the M different tissues (Hz)
	double psi;				//the local magnetic resonance offset (Hz)

	complex<double> eval_signal_2comp(double ro_1, double ro_2, double df_1, double df_2, double this_psi, double te);
	complex<double> eval_signal(double te);
};


MR_signal::MR_signal(int nr_tissue_comp){
	M=nr_tissue_comp;
	ro = std::vector<double>(10,0);
	df = std::vector<double>(10,0);
	psi=0;
}

complex<double> MR_signal::eval_signal_2comp(double ro_1, double ro_2, double df_1, double df_2, double this_psi, double te){
	//e^(ix) = cosx + i sinx
	M=2;
	ro[0]=ro_1;
	ro[1]=ro_2;
	df[0]=df_1;
	df[1]=df_2;
	psi=this_psi;
	return eval_signal(te);
}

complex<double> MR_signal::eval_signal(double te){
	double fact = 2.0*vnl_math::pi*te;
	complex<double> sum(0,0);
	complex<double> tmp2(0,0);
	for(int i=0;i<M;i++){
		tmp2 = complex<double>( cos(fact*df[i]), sin(fact*df[i]) );
		sum += ro[i]*tmp2;
	}
	complex<double> inhomog( cos(fact*psi),sin(fact*psi) );

	return sum*inhomog;
}


//------------------------------------------
//------------------------------------------
//------------------------------------------
void s_eval_function(int userIO_ID,int par_num)
{
	if (par_num == USERIO_CB_OK)
	{
		double r1 = double(userIOmanagement.get_parameter<float>(userIO_ID,0));
		double r2 = double(userIOmanagement.get_parameter<float>(userIO_ID,1));
		double df1 = double(userIOmanagement.get_parameter<float>(userIO_ID,2));
		double df2 = double(userIOmanagement.get_parameter<float>(userIO_ID,3));
		double psi = double(userIOmanagement.get_parameter<float>(userIO_ID,4));
		double te = 0.001*double(userIOmanagement.get_parameter<float>(userIO_ID,5));

		MR_signal s = MR_signal(2);
		complex<double> val = s.eval_signal_2comp(r1,r2,df1,df2,psi,te);
		cout<<val<<"\t"<<abs(val)<<"\t"<<arg(val)<<endl;
		//		userIOmanagement.show_message("MR signal:","Here we go MR Dixon...");
	}
}

//----------------------------------------------------------------
//--------------- IDEAL help-functions ---------------------------
//----------------------------------------------------------------

double g_imag_inner(int M, int n, vnl_vector<double> RO_tak, double c[2][3], double d[2][3])
{
	double ret=0;
	for(int j=0;j<M-1;j++)
	{
		ret += (RO_tak(2*j)*c[j][n] - RO_tak(2*j+1)*d[j][n]);
	}
	return ret;
}

double g_imag(double te[3], int M, int n, vnl_vector<double> RO_tak, double c[2][3], double d[2][3])
{
	return 2.0*vnl_math::pi*te[n]*g_imag_inner(M,n,RO_tak,c,d);
}

double g_real_inner(int M, int n, vnl_vector<double> RO_tak, double c[2][3], double d[2][3])
{
	double ret=0;
	for(int j=0;j<M-1;j++)
	{
		ret += -RO_tak(2*j)*d[j][n] - RO_tak(2*j+1)*c[j][n];
	}
	return ret;
}

double g_real(double te[3], int M, int n, vnl_vector<double> RO_tak, double c[2][3], double d[2][3])
{
	return 2.0*vnl_math::pi*te[n]*g_real_inner(M,n,RO_tak,c,d);
}

//------------------------------------------
//------------------------------------------
//------------------------------------------

void IDEAL_function(int userIO_ID,int par_num)
{
	if (par_num == USERIO_CB_OK)
	{
		int N = 3;		//number of echos
		int M = 2;		//number of tissue components (water and fat)
		//		double te[3] = {1.24,2.24,3.24};	//echo times for the three echos (s)
		//		double te[3] = {0.00124,0.00224,0.00324};	//echo times for the three echos (s) (FON 3P)
		double te[3] = {0.0013905,0.0026603,0.0039301};	//echo times for the three echos (s) (Amoar 3P EPI)
		//		double te[3] = {0.001,0.002,0.003};	//made up data....
		double df[2] = {0,210};						//chemical shift (Hz) for water and fat
		//		double df[2] = {0,125};						//made up data...
		double c[2][3];	//constant cos terms								
		double d[2][3];	//constant sin terms
		/*
		double r1 = 100;	//units of water
		double r2 = 10;		//units of fat
		double psi = 10;		//B0 field inhomog. (Hz)

		MR_signal mrs = MR_signal(M);
		complex<double> s0 = mrs.eval_signal_2comp(r1,r2,df[0],df[1],psi,te[0]);
		complex<double> s1 = mrs.eval_signal_2comp(r1,r2,df[0],df[1],psi,te[1]);
		complex<double> s2 = mrs.eval_signal_2comp(r1,r2,df[0],df[1],psi,te[2]);
		*/		

		//		vnl_vector<double> S_init(2*N); //6*1

		vnl_vector<double> S_tak(2*N); //6*1
		/*
		S_tak(0) = 110;	S_tak(3) = 10;	//Made up data....
		S_tak(1) = 100;	S_tak(4) = 14.5;
		S_tak(2) = 90;	S_tak(5) = 10;
		*/
		S_tak(0) = 2162-2047;	S_tak(3) = 2060-2047;	//Data from Amoar (3P EPI, MR503_3P_EPI_slc76_re1_te...)
		S_tak(1) = 1948-2047;	S_tak(4) = 2052-2047;
		S_tak(2) = 2001-2047;	S_tak(5) = 2125-2047;

		/*
		S_tak(0) = 2130-2047;	//re_1		//data from FON (1 fat pixel)
		S_tak(1) = 2150-2047;	//re_2
		S_tak(2) = 1952-2047;	//re_3
		S_tak(3) = 2104-2047;	//im_1
		S_tak(4) = 1999-2047;	//im_2
		S_tak(5) = 1942-2047;	//im_3
		*/
		/*
		S_tak(0) = real(s0);	//re_2
		S_tak(1) = real(s1);	//re_2
		S_tak(2) = real(s2);	//re_3
		S_tak(3) = imag(s0);	//im_1
		S_tak(4) = imag(s1);	//im_2
		S_tak(5) = imag(s2);	//im_3
		*/
		cout<<"S_tak="<<endl<<S_tak<<endl;

		vnl_vector<double> RO_tak(2*N); //6*1

		for(int j=0;j<M;j++){
			for(int n=0;n<N;n++){
				c[j][n] = cos(2.0*vnl_math::pi*df[j]*te[n]);
				d[j][n] = sin(2.0*vnl_math::pi*df[j]*te[n]);
			}
		}

		cout<<"c="<<endl;
		for(int n=0;n<N;n++){
			for(int j=0;j<M;j++){
				cout<<c[j][n]<<" ";
			}
			cout<<endl;
		}
		cout<<"d="<<endl;
		for(int n=0;n<N;n++){
			for(int j=0;j<M;j++){
				cout<<d[j][n]<<" ";
			}
			cout<<endl;
		}

		//---------- A ----------------------
		vnl_matrix<double> A(2*N,2*M); //6*4 for 3P-Dixon 

		for(int i=0;i<=N-1;i++){			//First N rows
			for(int j=0;j<=2*M-1;j=j+2){	//Even columns
				A(i,j)=c[j/2][i];			//Component id (0=water, 1=fat)
			}
			for(int j=1;j<=2*M-1;j=j+2){	//Odd columns
				A(i,j)=-d[j/2][i];			//Component id (0=water, 1=fat)
			}
		}

		for(int i=N;i<=2*N-1;i++){			//Following N rows
			for(int j=0;j<=2*M-1;j=j+2){	//Even columns
				A(i,j)=d[j/2][i-N];			//Component id (0=water, 1=fat)
			}
			for(int j=1;j<=2*M-1;j=j+2){	//Odd columns
				A(i,j)=c[j/2][i-N];			//Component id (0=water, 1=fat)
			}
		}
		cout<<"A="<<endl<<A<<endl;


		//---------- B ----------------------
		vnl_matrix<double> B(2*N,2*M+1);

		//first the part that is equal to A...
		for(int i=0;i<=2*N-1;i++){				//First N rows
			for(int j=1;j<=2*M;j++){			//Odd columns
				B(i,j)=A(i,j-1);				//Component id (0=water, 1=fat)
			}
		}

		//Then the part that is updated in each iteration step... (RO_tak has no values yet!)
		//		for(int i=0;i<=N-1;i++){					//First N rows
		//			B(i,0)=g_real(te,N,M,i,RO_tak,c,d);		//First column
		//		}
		//		for(int i=N;i<=2*N-1;i++){					//Following N rows
		//			B(i,0)=g_imag(te,N,M,i-N,RO_tak,c,d);	//First column
		//		}


		// --------------- First calculation of RO_tak ---------------

		vnl_matrix<double> tmp(2*M,2*M);
		vnl_matrix<double> tmp_inv(2*M,2*M);
		vnl_matrix<double> A_tmp(2*M,2*M);

		//Dicom tag 0008,0008 determines magnitude, imag, real, phase (M/R/I/P) image data...
		image_scalar<unsigned short,3> * re1_im = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		image_scalar<unsigned short,3> * im1_im = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,1)));
		image_scalar<unsigned short,3> * re2_im = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,2)));
		image_scalar<unsigned short,3> * im2_im = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,3)));
		image_scalar<unsigned short,3> * re3_im = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,4)));
		image_scalar<unsigned short,3> * im3_im = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,5)));

		image_scalar<unsigned short,3> w_im = image_scalar<unsigned short,3>(re1_im,0);
		image_scalar<unsigned short,3> f_im = image_scalar<unsigned short,3>(re1_im,0);
		image_scalar<unsigned short,3> psi_im = image_scalar<unsigned short,3>(re1_im,0);

		for(int m=0;m<re1_im->get_size_by_dim(2);m++){
			for(int l=0;l<re1_im->get_size_by_dim(1);l++){
				for(int k=0;k<re1_im->get_size_by_dim(0);k++){
					S_tak(0) = double(re1_im->get_voxel(k,l,m))-2047;	//re_1
					S_tak(1) = double(re2_im->get_voxel(k,l,m))-2047;	//re_2
					S_tak(2) = double(re3_im->get_voxel(k,l,m))-2047;	//re_3
					S_tak(3) = double(im1_im->get_voxel(k,l,m))-2047;	//im_1
					S_tak(4) = double(im2_im->get_voxel(k,l,m))-2047;	//im_2
					S_tak(5) = double(im3_im->get_voxel(k,l,m))-2047;	//im_3

					// --------------- First calculation of RO_tak ---------------
					tmp = A.transpose()*A;
					tmp_inv = vnl_matrix_inverse<double>(tmp);
					A_tmp = tmp_inv*A.transpose();
					RO_tak = A_tmp*S_tak;
					//		cout<<"RO_tak="<<endl<<RO_tak<<endl;


					//--- OBS använd RO_tak... bla för att skapa "g_imag_inner", B osv... 
					for(int i=0;i<=N-1;i++){					//First N rows
						B(i,0)=g_real(te,M,i,RO_tak,c,d);		//First column
					}
					for(int i=N;i<=2*N-1;i++){					//Following N rows
						B(i,0)=g_imag(te,M,i-N,RO_tak,c,d);	//First column
					}
					//		cout<<"B="<<endl<<B<<endl;



					// -------- Step (2, 3, 4)-->5 ------------

					double est_psi = 0;			//Start psi guess = 0
					double est_dpsi = 1000;		//very large starting value
					int i=1;
					vnl_vector<double> S_tak2(2*N); //6*1
					vnl_vector<double> S_tak_tmp(2*N); //6*1
					//		S_tak = S_init;

					double fact;

					double convergence_dpsi_limit = 1; //a value to small will not leed to convergance...
					int max_number_of_iterations = 6;

					while(abs(est_dpsi)>convergence_dpsi_limit && i<=max_number_of_iterations){
						//			cout<<endl<<"i="<<i<<" est_psi="<<est_psi<<" est_dpsi="<<est_dpsi<<" S_tak="<<S_tak<<endl;

						//------------- Step 2: y = ? ---------------------------

						// Set S_tak2 according to equations [B3] and [B4]
						for(int j=0;j<N;j++){
							S_tak2(j)	= S_tak(j) - g_imag_inner(M,j,RO_tak,c,d);
							S_tak2(j+N) = S_tak(j+N) + g_real_inner(M,j,RO_tak,c,d);
						}
						//			cout<<"S_tak2="<<S_tak2<<endl;

						vnl_vector<double> y(2*N); //6*1
						vnl_matrix<double> tmp2(2*M+1,2*M+1);
						vnl_matrix<double> tmp2_inv(2*M+1,2*M+1);

						tmp2 = B.transpose()*B;
						tmp2_inv = vnl_matrix_inverse<double>(tmp2);
						y = tmp2_inv*B.transpose()*S_tak2;
						//			cout<<"y="<<y<<endl;


						//------------- Step 3: Psi = ? ---------------------------

						est_dpsi = y(0);
						est_psi += est_dpsi;

						//------------- Step 4: next S_tak = ? ---------------------------

						fact = 2.0*vnl_math::pi*est_dpsi;		// eq [3] --> importanat to use S_init...
						S_tak_tmp(0) = S_tak(0)*cos(fact*te[0]) - S_tak(3)*(-sin(fact*te[0]));
						S_tak_tmp(3) = S_tak(3)*cos(fact*te[0]) + S_tak(0)*(-sin(fact*te[0]));
						S_tak_tmp(1) = S_tak(1)*cos(fact*te[1]) - S_tak(4)*(-sin(fact*te[1]));
						S_tak_tmp(4) = S_tak(4)*cos(fact*te[1]) + S_tak(1)*(-sin(fact*te[1]));
						S_tak_tmp(2) = S_tak(2)*cos(fact*te[2]) - S_tak(5)*(-sin(fact*te[2]));
						S_tak_tmp(5) = S_tak(5)*cos(fact*te[2]) + S_tak(2)*(-sin(fact*te[2]));

						S_tak = S_tak_tmp;
						//			cout<<"S_tak_after234="<<S_tak<<endl;
						i++;
					}

					//		cout<<endl<<"----> Done!!!   i="<<i<<"\t est_psi="<<est_psi<<"  S_tak="<<S_tak<<endl;
					//		cout<<i<<"("<<est_psi<<")";

					//----- temporary, just to get some w/f output... ------
					RO_tak = A_tmp*S_tak;
					psi_im.set_voxel(k,l,m,est_psi+10000);
					w_im.set_voxel(k,l,m,sqrt(RO_tak(0)*RO_tak(0)+RO_tak(1)*RO_tak(1)));
					f_im.set_voxel(k,l,m,sqrt(RO_tak(2)*RO_tak(2)+RO_tak(3)*RO_tak(3)));
				}
			}
		}
		psi_im.save_to_VTK_file("c:\\Joel\\TMP\\psi_im.vtk");
		w_im.save_to_VTK_file("c:\\Joel\\TMP\\w_im.vtk");
		f_im.save_to_VTK_file("c:\\Joel\\TMP\\f_im.vtk");


		// -------- Step 5 ------------	 //	smooth the "Psi-image"


		// -------- Step 6 ------------
		//calculate S_tak = f(S_init, est_psi)....

		RO_tak = A_tmp*S_tak;
		cout<<"*******************"<<endl;
		cout<<"RO_tak= "<<RO_tak<<endl;
		cout<<"ro1= "<<sqrt(RO_tak(0)*RO_tak(0)+RO_tak(1)*RO_tak(1))<<endl;
		cout<<"ro2= "<<sqrt(RO_tak(2)*RO_tak(2)+RO_tak(3)*RO_tak(3))<<endl;
		//		cout<<"est_psi="<<est_psi<<endl;

	}
}
//---------------------------------
//---------------------------------
//---------------------------------
//---------------------------------
//----------------------------------------------------------------
//--------------- IDEAL help-functions ---------------------------
//----------------------------------------------------------------

double g_imag_inner2(int M, int n, vnl_vector<double> RO_tak, vnl_matrix<double> c, vnl_matrix<double> d)
{
	double ret=0;
	for(int j=0;j<M;j++)
	{
		ret += (RO_tak(2*j)*c(j,n) - RO_tak(2*j+1)*d(j,n));
	}
	return ret;
}
/*
double g_imag(double te[3], int M, int n, vnl_vector<double> RO_tak, double c[2][3], double d[2][3])
{
return 2.0*vnl_math::pi*te[n]*g_imag_inner(M,n,RO_tak,c,d);
}
*/
double g_real_inner2(int M, int n, vnl_vector<double> RO_tak, vnl_matrix<double> c, vnl_matrix<double> d)
{
	double ret=0;
	for(int j=0;j<M;j++)
	{
		ret += RO_tak(2*j)*d(j,n) + RO_tak(2*j+1)*c(j,n);
	}
	return -ret;
}
/*
double g_real(double te[3], int M, int n, vnl_vector<double> RO_tak, double c[2][3], double d[2][3])
{
return 2.0*vnl_math::pi*te[n]*g_real_inner(M,n,RO_tak,c,d);
}
*/

double get_rand(double min, double max){
	return min + (max-min)*(double(rand())/double(RAND_MAX));
}


// Second implementation of iterative "dixon-method" reported by Reeder in 2004.
void IDEAL_function2(int userIO_ID,int par_num) 
{
	if (par_num == USERIO_CB_OK)
	{
		int M = 2;		//number of tissue components (water and fat) (j-index)
		int N = 5;		//number of echos
		//ÖÖÖ
		vnl_vector<double> te(N); //3*1, printed as 1*3 echo times for the N echoes...
		vnl_vector<double> te_t2pi(N); //te "times 2 PI"... for speedup...
		vnl_vector<double> df(M); //2*1, printed as 1*2 resonance frequency offsets for the M species (0=water, 1=fat)
		vnl_matrix<double> c(M,N); //2*3 indexed M,N (j,n) according to article (cos constants)
		vnl_matrix<double> d(M,N); //2*3 indexed M,N (j,n) according to article (sin constants)
		vnl_matrix<double> A(2*N,2*M); //6*4 for 3P-Dixon 
		vnl_matrix<double> A_tmp(2*M,2*M); //Matrix operation cache...
		vnl_matrix<double> B(2*N,2*M+1); //6*5 for 3P-Dixon 
		vnl_matrix<double> B_tmp(2*M+1,2*N); //Matrix operation cache... 
		vnl_vector<double> S_tak(2*N); //6*1 printed as 1*6... (complex signal intensities)
		vnl_vector<double> RO_tak(2*M); //4*1, printed as 1*4... (complex species intensities, i.e water/fat)
		double est_dpsi = 1000;
		double est_psi = 0;
		vnl_vector<double> S_tak2(2*N); //6*1 printed as 1*6... (complex signal intensities, iteratively updated...)
		vnl_vector<double> y(2*M+1); //5*1 printed as 1*5... (delta_psi, delta_ro...)

//		double te_start = 0.00124;	//3P dixon, multiple breath holds....
//		double te_step = 0.001;		//Jag lyckas verkligen inte få till denna.....
//		double te_start = 0.0013905;	//3P dixon, EPI
//		double te_step = 0.0012698;
		double te_start = 0.0013812;	//5P dixon, EPI
		double te_step = 0.0012512;

		for(int n=0;n<N;n++){
			te(n) = te_start + double(n)*te_step;
			te_t2pi(n) = te(n)*2.0*vnl_math::pi;
		}

		df(0)=0;
		df(1)=-210;

		// ------- Parameters for simulated data ------- 
		double r1 = 100;	//units of water
		double r2 = 100;	//units of fat
		double psi = 10;	//B0 field inhomog. (Hz)

		MR_signal mrs = MR_signal(M);
		complex<double> s[5];

		for(int n=0;n<N;n++){
			s[n] = 0;//mrs.eval_signal_2comp(r1,r2,df(0),df(1),psi,te(n));
		}

		for(int j=0;j<M;j++){
			for(int n=0;n<N;n++){
				c(j,n) = cos(te_t2pi(n)*df(j));
				d(j,n) = sin(te_t2pi(n)*df(j));
			}
		}

		for(int i=0;i<N;i++){	//for the first N rows in A...
			for(int j=0;j<2*M;j=j+2){
				A(i,j)=c(j/2,i);
			}
			for(int j=1;j<2*M;j=j+2){
				A(i,j)=-d(j/2,i);
			}
		}
		for(int i=N;i<2*N;i++){	//for the first N rows in A...
			for(int j=0;j<2*M;j=j+2){
				A(i,j)=d(j/2,i-N);
			}
			for(int j=1;j<2*M;j=j+2){
				A(i,j)=c(j/2,i-N);
			}
		}
		A_tmp = (vnl_matrix_inverse<double>(A.transpose()*A))*A.transpose();


		//B --> first the part that is equal to A... (the first column needs ro to be set...)
		for(int i=0;i<=2*N-1;i++){				//First N rows
			for(int j=1;j<=2*M;j++){			//Odd columns
				B(i,j)=A(i,j-1);				//Component id (0=water, 1=fat)
			}
		}
		cout<<"te="<<endl<<te<<endl;
		cout<<"c="<<endl<<c<<endl;
		cout<<"d="<<endl<<d<<endl;
		cout<<"A="<<endl<<A<<endl;
		cout<<"B="<<endl<<B<<endl;

		double error = 0.01;
		for(int n=0;n<N;n++){
			S_tak(n)	= real( mrs.eval_signal_2comp(r1,r2,df(0),df(1),psi,te(n)) );	//re_-->(n)
			S_tak(n+N)	= imag( mrs.eval_signal_2comp(r1,r2,df(0),df(1),psi,te(n)) );	//im_-->(n)
			//S_tak(n)	= get_rand(1-error,1+error)*real( mrs.eval_signal_2comp(r1,r2,df(0),df(1),psi,te(n)) );	//re_-->(n)
			//S_tak(n+N)	= get_rand(1-error,1+error)*imag( mrs.eval_signal_2comp(r1,r2,df(0),df(1),psi,te(n)) );	//im_-->(n)
		}

		/*
		S_tak(0) = 2130-2047;	//re_1		//data from FON (1 fat pixel)
		S_tak(1) = 2150-2047;	//re_2
		S_tak(2) = 1952-2047;	//re_3
		S_tak(3) = 2104-2047;	//im_1
		S_tak(4) = 1999-2047;	//im_2
		S_tak(5) = 1942-2047;	//im_3

		S_tak(0) = 1945-2047;	//re_1		//data from FON (1 muscle pixel)
		S_tak(1) = 2143-2047;	//re_2
		S_tak(2) = 1954-2047;	//re_3
		S_tak(3) = 2058-2047;	//im_1
		S_tak(4) = 2014-2047;	//im_2
		S_tak(5) = 2096-2047;	//im_3
		*/

//		double factor;
		complex<double> a[5];
		//		complex<double> a1;
		//		complex<double> a2;
		//		complex<double> a3;
		//		complex<double> a4;
		int nr;


		image_scalar<unsigned short,3> * re_im[5];
		image_scalar<unsigned short,3> * im_im[5];
		
		//------------------------- Image loop -------------------------
		//Dicom tag 0008,0008 determines magnitude, imag, real, phase (M/R/I/P) image data...
		for(int n=0;n<N;n++){
			re_im[n] = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,2*n)));
			im_im[n] = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,2*n+1)));
		}

		image_scalar<unsigned short,3> mask = image_scalar<unsigned short,3>(re_im[0],0);
		image_scalar<unsigned short,3> w_im = image_scalar<unsigned short,3>(re_im[0],0);
		image_scalar<unsigned short,3> f_im = image_scalar<unsigned short,3>(re_im[0],0);
		image_scalar<unsigned short,3> psi_im = image_scalar<unsigned short,3>(re_im[0],0);
		image_scalar<unsigned short,3> nr_iter_im = image_scalar<unsigned short,3>(re_im[0],0);

		double re_1;
		double im_1;
		for(int m=0;m<re_im[0]->get_size_by_dim(2);m++){
			for(int l=0;l<re_im[0]->get_size_by_dim(1);l++){
				for(int k=0;k<re_im[0]->get_size_by_dim(0);k++){
					re_1 = re_im[0]->get_voxel(k,l,m)-2047;
					im_1 = im_im[0]->get_voxel(k,l,m)-2047;
					mask.set_voxel(k,l,m,sqrt(re_1*re_1+im_1*im_1));
				}
			}
		}

		double tmp_g_r=0;
		double tmp_g_i=0;

		for(int m=0;m<1;m++){
//		for(int m=0;m<re_im[0]->get_size_by_dim(2);m++){
			for(int l=0;l<re_im[0]->get_size_by_dim(1);l++){
				cout<<".";
				for(int k=0;k<re_im[0]->get_size_by_dim(0);k++){
					if(mask.get_voxel(k,l,m)>30){

						for(int n=0;n<N;n++){
							S_tak(n) = double(re_im[n]->get_voxel(k,l,m))-2047;	//re_(1-N)
							S_tak(n+N) = double(im_im[n]->get_voxel(k,l,m))-2047;	//im_(1-N)
						}

						// ---------------- STEP 1 -------------------
						// ------- ro = (A^T*A)^-1 * A^T * S_tak -----

						RO_tak = A_tmp*S_tak;

						est_dpsi=1000;
						est_psi=0;
						nr=0;
						while(abs(est_dpsi)>1 && nr<8){
							nr++;

							// ---------------- STEP 2 -------------------
							// -------------- est_dpsi = ? ---------------

							for(int n=0;n<N;n++){
								// ...update B... //JK OBS ÖÖÖ - is this data supposed to be updated within the while-loop? 
								tmp_g_r = g_real_inner2(M,n,RO_tak,c,d);
								tmp_g_i = g_imag_inner2(M,n,RO_tak,c,d);

								B(n,0)		= te_t2pi(n)*tmp_g_r;
								B(n+N,0)	= te_t2pi(n)*tmp_g_i;

								// ...update S_tak2...	// is this data supposed to be updated within the while-loop? 
								S_tak2(n)	= S_tak(n) - tmp_g_i;
								S_tak2(n+N)	= S_tak(n+N) + tmp_g_r;
							}

							B_tmp = (vnl_matrix_inverse<double>(B.transpose()*B))*B.transpose();
							y = B_tmp*S_tak2;
							est_dpsi = y(0);

							// ---------------- STEP 3 -------------------
							// -------------- est_psi = ? ----------------
							est_psi += est_dpsi;

							//This was an option that was tested: to update RO_tak from the delta_RO values in y.
							//RO_tak(0) += y(1);
							//RO_tak(1) += y(2);
							//RO_tak(2) += y(3);
							//RO_tak(3) += y(4);

							//cout<<"nr="<<nr<<endl;
							//cout<<"S_tak="<<endl<<S_tak<<endl;
							//cout<<"RO_tak="<<endl<<RO_tak<<endl;
							//cout<<"S_tak2="<<endl<<S_tak2<<endl;
							//cout<<"y="<<endl<<y<<endl;
							//cout<<"est_dpsi="<<endl<<est_dpsi<<endl;

							// ---------------- STEP 4 -------------------
							// ----------- Recalculate S_tak -------------

//							factor = 2*vnl_math::pi*est_dpsi;   //the use of "dspi" is important... (se email from S.B. Reader)

							for(int n=0;n<N;n++){
								a[n] = complex<double>(S_tak(n), S_tak(n+N));
								s[n] = complex<double>( cos( te_t2pi(n)*est_dpsi ), -sin( te_t2pi(n)*est_dpsi ) );
								a[n] = a[n]*s[n];
								S_tak(n) = real(a[n]);		//re_1-N
								S_tak(n+N) = imag(a[n]);	//im_1-N
							}

							//JK OBS ÖÖÖ - Ska inte RO_tak också räknas om från nya S_tak?? (alt. from dela_ro via y...)
							RO_tak = A_tmp*S_tak;

						}

						//cout<<"w="<<sqrt(RO_tak(0)*RO_tak(0)+RO_tak(1)*RO_tak(1))<<endl;
						//cout<<"f="<<sqrt(RO_tak(2)*RO_tak(2)+RO_tak(3)*RO_tak(3))<<endl;
						//cout<<"est_psi="<<est_psi<<endl;

						psi_im.set_voxel(k,l,m,est_psi+1000);
						w_im.set_voxel(k,l,m,sqrt(RO_tak(0)*RO_tak(0)+RO_tak(1)*RO_tak(1)));
						f_im.set_voxel(k,l,m,sqrt(RO_tak(2)*RO_tak(2)+RO_tak(3)*RO_tak(3)));
						nr_iter_im.set_voxel(k,l,m,nr);

					}else{ //if mask-magnitude < 100
						psi_im.set_voxel(k,l,m,1000);
						w_im.set_voxel(k,l,m,0);
						f_im.set_voxel(k,l,m,0);
						nr_iter_im.set_voxel(k,l,m,0);
					}
				}
			}
		}
		psi_im.save_to_VTK_file("c:\\Joel\\Code\\Platinum2LibBin\\Data\\_1psi_im.vtk");
		w_im.save_to_VTK_file("c:\\Joel\\Code\\Platinum2LibBin\\Data\\_2w_im.vtk");
		f_im.save_to_VTK_file("c:\\Joel\\Code\\Platinum2LibBin\\Data\\_3f_im.vtk");
		nr_iter_im.save_to_VTK_file("c:\\Joel\\Code\\Platinum2LibBin\\Data\\_4nr_im.vtk");
		mask.save_to_VTK_file("c:\\Joel\\Code\\Platinum2LibBin\\Data\\_5mask_im.vtk");
	}
}

//retrieve command-line arguments
int visid = -1;
int arg(int argc, char **argv, int &i) {
	if (argv[i][1] == 'v') {
		if (i+1 >= argc) return 0;
		visid = atoi(argv[i+1]);
		i += 2;
		return 2;
	}
	return 0;
}
/*
void add_demo_image (int userIO_ID,int par_num)
    {
    if (par_num == USERIO_CB_OK)
        {
        image_integer<unsigned char,3>* demo_image = new image_integer<unsigned char,3>(20,20,20,NULL);
        //demo_image->erase();
        demo_image->testpattern();

        datamanagement.add(demo_image);
        }
    }


image_scalar<unsigned char,3> * im1 = new image_scalar<unsigned char,3>(10,10,10);
im1->origin[0]=0;
im1->origin[1]=0;
im1->origin[2]=0;
im1->set_voxel_resize(1,1,1);

matrix_generator mg;
im1->direction = mg.get_rot_matrix_3D(0.25*PI,0.01*PI,0.1*PI);
cout<<"im1->direction="<<im1->direction<<endl;
cout<<"im1->get_physical_pos_for_voxel(5,5,5)="<<im1->get_physical_pos_for_voxel(5,5,5)<<endl;


im1->set_value_to_all_voxels(50);
//		im1->set_value_to_all_voxels(255);
im1->set_value_to_voxels_in_region(0,0,0,1,1,1,100);
im1->set_value_to_voxels_in_region(2,0,0,1,5,1,150);
im1->set_value_to_voxels_in_region(5,5,5,4,1,1,200);

im1->image_has_changed(true);
im1->save_to_VTK_file("C:\\Joel\\TMP\\im1.vtk");


image_scalar<unsigned char,3> * im2 = new image_scalar<unsigned char,3>(100,100,100);
im2->origin[0]=0;
im2->origin[1]=0.2;
im2->origin[2]=0;
im2->set_voxel_resize(0.1,0.1,0.1);
im2->set_value_to_all_voxels(0);


im2->image_has_changed(true);
im2->save_to_VTK_file("C:\\Joel\\TMP\\im2.vtk");


//		im1->resample_into_this_image_NN(im2); //this removes all previous values....
im2->interpolate_trilinear_3D_vxl(im1);

im2->image_has_changed(true);
im2->save_to_VTK_file("C:\\Joel\\TMP\\im3.vtk");

im1->erase();
im2->erase();
}
}
*/

void resampling_function (int userIO_ID,int par_num)
{
	if (par_num == USERIO_CB_OK)
	{
		cout<<" Hej - resampling_function - 0"<<endl;
		//steget.... voxel_resize/=spacing_min_norm
		//i funktionen (void image_general<ELEMTYPE, IMAGEDIM>::set_parameters(itk::SmartPointer< itk::Image<ELEMTYPE, IMAGEDIM > > &i))
		//gör att användandet av absoluta koordinater får vänta tills rendreringen görs om!

		//FruitMR201 --> 50, 86, 7 --> 25, 64, 7 in FruitMR301

		/*
		image_scalar<unsigned short,3>	* f2 = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		f2->get_geometry_from_dicom_file("c:\\Joel\\TMP\\FruitMR201.dcm");

		image_scalar<unsigned short,3>	* f3 = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,1)));
		f3->get_geometry_from_dicom_file("c:\\Joel\\TMP\\FruitMR301.dcm");

		f2->interpolate_trilinear_3D_vxled(f3);
		f2->save_to_VTK_file("c:\\Joel\\TMP\\_3res.vtk");
		*/
		/*
		image_scalar<unsigned short,3>	* r = new image_scalar<unsigned short,3>(50,50,50);
		r->set_geometry(0,0,0,1,1,1,0,0,0);

		image_scalar<unsigned short,3>	* s = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		s->set_geometry(0,0,0,1,2,1,0,0,0);

		r->interpolate_tricubic_3D_libtricubic(s);
		r->save_to_VTK_file("c:\\Joel\\TMP\\_3res_libtricubic_latest.vtk");

		*/
		/*
		// *** REF *** 
		//		image_scalar<unsigned short,3>	* r = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		image_scalar<unsigned short,3>	* r = new image_scalar<unsigned short,3>(50,50,50);
		r->set_geometry(0,0,0,1,1,1,0,0,0);
		r->set_value_to_all_voxels(10);
		r->save_to_VTK_file("c:\\Joel\\TMP\\_1ref.vtk");


		// *** SRC ***
		image_scalar<unsigned short,3>	* s = new image_scalar<unsigned short,3>(20,20,10);
		s->set_geometry(10,10,0,1,1,3,4.56*PI,0.21*PI,0.1*PI);
		for(int z=0;z<10;z++){
		s->set_value_to_voxels_in_region(0,0,z,20,20,1,(z+1)*10);
		}

		s->save_to_VTK_file("c:\\Joel\\TMP\\_2src.vtk");

		r->interpolate_tricubic_3D_libtricubic(s);
		r->save_to_VTK_file("c:\\Joel\\TMP\\_3res_libtricubic_latest.vtk");
		*/
		/*
		ref->interpolate_trilinear_3D_vxl(src);
		ref->save_to_VTK_file("c:\\Joel\\TMP\\_3res.vtk");

		src->rotate(0.25*PI,0,0);
		ref->interpolate_trilinear_3D_vxl(src);
		ref->save_to_VTK_file("c:\\Joel\\TMP\\_4res.vtk");

		src->rotate(0.25*PI,0,0);
		ref->interpolate_trilinear_3D_vxl(src);
		ref->interpolate_tricubic_3D_libtricubic(src);
		ref->save_to_VTK_file("c:\\Joel\\TMP\\_5res.vtk");
		*/




		if(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)>0)
		{

			//MR201-NEJ-MPRAGE-Sagittal
			image_scalar<unsigned short,3>	* r = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
			r->get_geometry_from_dicom_file("c:\\Joel\\TMP\\MR201.dcm");
			//			r->print_geometry();
			r->save_to_VTK_file("c:\\Joel\\TMP\\_1ref.vtk");

			//JK testing....


			//MR301-NEJ-Dual-TE21
			image_scalar<unsigned short,3>	* s = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,1)));
			s->get_geometry_from_dicom_file("c:\\Joel\\TMP\\MR301_TE21.dcm");
			//			s->print_geometry();
			s->save_to_VTK_file("c:\\Joel\\TMP\\_2src.vtk");

			//			image_scalar<unsigned short,3>	* t1 = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,2)));
			//			t1->print_geometry();
			//			t1->get_geometry_from_dicom_file("c:\\Joel\\TMP\\MR201.dcm");
			//			t1->save_to_VTK_file("c:\\Joel\\TMP\\_4t1.vtk");

			//			image_scalar<unsigned short,3>	* t2 = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,3)));
			//			t2->print_geometry();
			//			t2->get_geometry_from_dicom_file("c:\\Joel\\TMP\\MR301_TE21.dcm");
			//			t2->save_to_VTK_file("c:\\Joel\\TMP\\_4t2.vtk");

			//MR401-NEJ-T2*w-Axial
			//0018,0050  Slice Thickness: 5.0 	dz = 5.0;		//--> Egentligen 5.5 spacing...
			//			image_scalar<unsigned short,3>	* ref = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
			//			ref->get_geometry_from_dicom_file("c:\\Joel\\TMP\\MR401.dcm");
			//			ref->save_to_VTK_file("c:\\Joel\\TMP\\_1ref.vtk");


			//MR701-NEJ-T1wGd-Axial
			//			image_scalar<unsigned short,3>	* src = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,1)));
			//			src->get_geometry_from_dicom_file("c:\\Joel\\TMP\\MR701.dcm");
			//			src->save_to_VTK_file("c:\\Joel\\TMP\\_2src.vtk");


			//			r->interpolate_tricubic_3D_libtricubic(s);
			//			r->interpolate_trilinear_3D_vxl(s);
			r->save_to_VTK_file("c:\\Joel\\TMP\\_3res.vtk");

		}else
		{
			cout<<"*** Warning!!! No image loaded..."<<endl;
		}


		/*
		// *** From Google-Code-Wiki ***
		//image_binary * my_binary_pointer = dynamic_cast<image_binary<3>* >(datamanagement.get_image(id));

		// *** From Histo2d-Example ***
		//JK - Detta verkar inte funka då guaranteed_cast "tar bort" den gamla pekaren om castningen lyckas!
		//image_scalar<unsigned char,3> * result = guaranteed_cast<image_scalar,unsigned char,3>(truth);

		//		image_scalar<unsigned char,3> * ref = guaranteed_cast<image_scalar,unsigned char,3>(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		//		image_scalar<unsigned char,3> * src = guaranteed_cast<image_scalar,unsigned char,3>(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,1)));
		//		image_scalar<unsigned char,3> * res = guaranteed_cast<image_scalar,unsigned char,3>(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		//		res->set_value_to_all_voxels(0);


		//		image_base * ref = datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0));

		//		image_base * ref = datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0));

		image_scalar<char,3>			* r2 = dynamic_cast<image_scalar<char,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		image_scalar<unsigned char,3>	* r3 = dynamic_cast<image_scalar<unsigned char,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		image_scalar<short,3>			* r4 = dynamic_cast<image_scalar<short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		image_scalar<unsigned short,3>	* r5 = dynamic_cast<image_scalar<unsigned short,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		image_scalar<long,3>			* r6 = dynamic_cast<image_scalar<long,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));
		image_scalar<int,3>	* r7 = dynamic_cast<image_scalar<int,3>* >(datamanagement.get_image(userIOmanagement.get_parameter<imageIDtype>(userIO_ID,0)));

		if(r2==NULL)
		cout<<"ref2==NULL"<<endl;
		if(r3==NULL)
		cout<<"ref3==NULL"<<endl;
		if(r4==NULL)
		cout<<"ref4==NULL"<<endl;
		if(r5==NULL)
		cout<<"ref5==NULL"<<endl;
		if(r6==NULL)
		cout<<"ref6==NULL"<<endl;
		if(r7==NULL)
		cout<<"ref7==NULL"<<endl;

		r5->save_to_VTK_file("C:\\Joel\\TMP\\_resampled05.vtk");
		*/
		cout<<" Hej - resampling_function - 1"<<endl;

	}
}



double args[9] = {
  20,20, 50,200, 100,20, 200,200, 0};
const char* name[9] = {
  "X0", "Y0", "X1", "Y1", "X2", "Y2", "X3", "Y3", "rotate"};

int points;


void push_cb(int event_x, int event_y) {
	cout<<"push_cb(Fl_Widget* o, void*) (x,y)="<<event_x<<","<<event_y<<endl;
}

class Drawing : public Fl_Widget {

	class point2D{
	public:
		int x;
		int y;
		point2D(int x2, int y2)
		{
			x=x2;
			y=y2;
		}
	};

private:
	void draw() {
		fl_clip(x(),y(),w(),h());
		fl_color(FL_GRAY);
		fl_rectf(x(),y(),w(),h(),FL_YELLOW);
		fl_push_matrix();
		if (args[8]) {
			fl_translate(x()+w()/2.0, y()+h()/2.0);
			fl_rotate(args[8]);
			fl_translate(-(x()+w()/2.0), -(y()+h()/2.0));
		}
		fl_translate(x(),y());
		if (!points) {
			fl_color(FL_WHITE);
			fl_begin_complex_polygon();
			fl_curve(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7]);
			fl_end_complex_polygon();
		}
		fl_color(FL_BLACK);
		fl_begin_line();
		fl_vertex(args[0],args[1]);
		fl_vertex(args[2],args[3]);
		fl_vertex(args[4],args[5]);
		fl_vertex(args[6],args[7]);
		fl_end_line();
		fl_color(points ? FL_WHITE : FL_RED);
		points ? fl_begin_points() : fl_begin_line();
		fl_curve(args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7]);
		points ? fl_end_points() : fl_end_line();
		fl_pop_matrix();
		fl_pop_clip();
	}

	int handle(int event){

		switch (event){
		case FL_PUSH:
			push_cb(Fl::event_x(), Fl::event_y());
			break;
			/*        case FL_DRAG:
			drag_dx = Fl::event_x() - mouse_pos[0];
			drag_dy = Fl::event_y() - mouse_pos[1];

			if (drag_dx !=0 || drag_dy !=0)
			{
			if(Fl::event_button() == FL_LEFT_MOUSE){
			do_callback(CB_ACTION_DRAG_PASS);
			}
			if(Fl::event_button() == FL_MIDDLE_MOUSE){
			do_callback(CB_ACTION_DRAG_PAN);
			}
			if(Fl::event_button() == FL_RIGHT_MOUSE){
			do_callback(CB_ACTION_DRAG_ZOOM);
			}            
			}
			break;
			*/
		default:
			//            callback_action=0;
			return 0;

		}
		//    mouse_pos[0]=Fl::event_x();
		//    mouse_pos[1]=Fl::event_y();

		//   callback_action=0; //better safe than sorry
		return 1;
	}

public:
	std::vector<point2D> points2D;

	Drawing(int X,int Y,int W,int H) : Fl_Widget(X,Y,W,H)
	{
		points2D = std::vector<point2D>();
	}

	void add_point(int x,int y)
	{
		point2D p = point2D(x,y);
		points2D.push_back(p);
	}

};

Drawing *d;

void points_cb(Fl_Widget* o, void*) {
  points = ((Fl_Toggle_Button*)o)->value();
  d->redraw();
}

void slider_cb(Fl_Widget* o, void* v) {
  Fl_Slider* s = (Fl_Slider*)o;
  args[long(v)] = s->value();
  d->redraw();
}




void FLTK_testing (int userIO_ID,int par_num)
{
	if (par_num == USERIO_CB_OK)
	{
		//int main(int argc, char** argv) {
		Fl_Double_Window window(300,555);
		Drawing drawing(10,10,280,280);
		d = &drawing;
		//  20,20, 50,200, 100,20, 200,200, 0};
		d->add_point(20,20); 
		d->add_point(50,200); 
		d->add_point(100,20); 
		d->add_point(200,200); 


		int y = 300;
		for (int n = 0; n<9; n++) {
			Fl_Slider* s = new Fl_Hor_Value_Slider(50,y,240,25,name[n]); 
			y += 25;
			s->minimum(0); 
			s->maximum(280);
			if (n == 8) 
				s->maximum(360);
			s->step(1);
			s->value(args[n]);
			s->align(FL_ALIGN_LEFT);
			s->callback(slider_cb, (void*)n);
		}
		Fl_Toggle_Button but(50,y,50,25,"points");
		but.callback(points_cb);

		window.end();
		window.show();
		Fl::run();
		//}
	}
}

class A
{
public:
	A(){cout<<"A-constructor"<<endl;};
	void f(){cout<<"A-f()"<<endl;};
	virtual void g(){cout<<"A-g()"<<endl;};
};
class B:A
{
public:
	B():A(){cout<<"B-constructor"<<endl;};
	void f(){cout<<"B-f()"<<endl;};
	virtual void g(){cout<<"B-g()"<<endl;};
};


void LIST_testing (int userIO_ID,int par_num)
{
	if (par_num == USERIO_CB_OK)
	{
		points_seq_func1D<float,float> p = points_seq_func1D<float,float>();
		p.insert(0,0);
		p.insert(1,1);
		p.insert(2,2);
		p.insert(3,2);
		p.insert(4,1);
		p.printdata();

		float *y2 = new float[100];
		y2 = p.get_spline_derivatives();

		for(int i=0;i<p.size()+1;i++)
			cout<<y2[i]<<endl;

		for(float i=-1;i<=p.size();i=i+0.5)
		{
			cout<<i;
			cout<<"\t"<<p.get_value_interp_spline(i,y2);
			cout<<"\t"<<p.get_value_interp_linear(i)<<endl;
		}

		A a;
		a.f();
		a.g();

		B b;
		b.f();
		b.g();

	}
}



int main(int argc, char *argv[])
{
	//start up Platinum
	platinum_init();

	//set up main window

	//const int w_margin=15*2;
	//int windowwidth = 800-w_margin;
	//int windowheight = 600-w_margin;

	Fl_Window window(1200,750);

	//prepare Platinum for userIO creation
	platinum_setup(window);

	// *** begin userIO control definitions ***
	/*
	int create_vol_demo_ID=userIOmanagement.add_userIO("Add demo image",add_demo_image,"Add");
	userIOmanagement.finish_userIO(create_vol_demo_ID);


	int resampling_test_ID=userIOmanagement.add_userIO("Resampling Testing...",resampling_function,"Resample");
	userIOmanagement.add_par_image(resampling_test_ID,"Input Ref");
	userIOmanagement.add_par_image(resampling_test_ID,"Input Src");
	userIOmanagement.add_par_image(resampling_test_ID,"Input Test1");
	userIOmanagement.add_par_image(resampling_test_ID,"Input Test2");
	//    userIOmanagement.add_par_int_box(resampling_test_ID,"Radius",4,1);
	userIOmanagement.finish_userIO(resampling_test_ID);


	int resampling_test_ID=userIOmanagement.add_userIO("Resampling Testing...",resampling_function,"Resample");
	userIOmanagement.add_par_image(resampling_test_ID,"Input Ref");

	int s_ID=userIOmanagement.add_userIO("MR signal testing",s_eval_function,"Calc");
	userIOmanagement.add_par_float(s_ID,"ro_1 (0-1)");
	userIOmanagement.add_par_float(s_ID,"ro_2 (0-1)");
	userIOmanagement.add_par_float(s_ID,"df_1 (0-300) Hz",300,0);
	userIOmanagement.add_par_float(s_ID,"df_2 (0-300) Hz",300,0);
	userIOmanagement.add_par_float(s_ID,"psi  (0-100) Hz",100,0);
	userIOmanagement.add_par_float(s_ID,"te   (0-10)  ms",10.0,0);
	userIOmanagement.finish_userIO(s_ID);


	int IDEAL_ID=userIOmanagement.add_userIO("IDEAL testing",IDEAL_function2,"Exec");
	userIOmanagement.add_par_image(IDEAL_ID,"Re1");
	userIOmanagement.add_par_image(IDEAL_ID,"Im1");
	userIOmanagement.add_par_image(IDEAL_ID,"Re2");
	userIOmanagement.add_par_image(IDEAL_ID,"Im2");
	userIOmanagement.add_par_image(IDEAL_ID,"Re3");
	userIOmanagement.add_par_image(IDEAL_ID,"Im3");
	userIOmanagement.add_par_image(IDEAL_ID,"Re4");
	userIOmanagement.add_par_image(IDEAL_ID,"Im4");
	userIOmanagement.add_par_image(IDEAL_ID,"Re5");
	userIOmanagement.add_par_image(IDEAL_ID,"Im5");
	userIOmanagement.finish_userIO(IDEAL_ID);
	*/

	int curve_ID=userIOmanagement.add_userIO("FLTK curve testing",FLTK_testing,"RUN");
	userIOmanagement.finish_userIO(curve_ID);

	int list_test_ID=userIOmanagement.add_userIO("List testing",LIST_testing,"RUN");
	userIOmanagement.finish_userIO(list_test_ID);

	// *** end userIO control definitions ***

	//finish the window creation
	window.end();

	window.show(argc, argv);

	return Fl::run();
}
