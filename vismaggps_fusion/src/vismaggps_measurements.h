/*
 * vismaggps_measurements.h
 *
 *  Created on: Sep 30, 2010
 *      Author: sweiss
 */

#ifndef VISMAGGPS_MEASUREMENTS_H_
#define VISMAGGPS_MEASUREMENTS_H_

#include <sensor_fusion_core/measurement.h>
#ifndef VICONTEST
	#include "vismaggps.h"
#else
	#include "vismaggps_vicontest.h"
#endif
namespace Sensors
{
  enum Sensor
  {
     VISION=1,
     MAG=2,
     GPS=3,
     VISMAGGPS=4
  };
}
typedef Sensors::Sensor Sensor;

class VisMagGPSMeasurements: public Measurements
{
public:
	// measurements
	Eigen::Quaternion<double> q_cv_;	// attitude camera -> vision frame
	Eigen::Matrix<double, 3, 1> p_vc_;	// position camera -> vision frame
	Eigen::Matrix<double, 3, 1> p_m_;	// mag direction vector in mag frame
	Eigen::Matrix<double, 3, 1> p_wg_;	// position world -> GPS sensor
	Eigen::Matrix<double, 2, 1> v_wg_;	// velocity world -> GPS sensor
	VisMagGPSMeasurements()
	{
		addHandler(Sensors::VISMAGGPS, new VisMagGPSHandler(this));
		poseFilter_.registerCallback(&VisMagGPSMeasurements::Config, this);
		q_cv_ = Eigen::Quaternion<double>::Identity();
		p_vc_ = Eigen::Matrix<double,3,1>::Constant(0);
		p_m_ = Eigen::Matrix<double,3,1>::Constant(0);
		p_wg_ = Eigen::Matrix<double,3,1>::Constant(0);
		v_wg_ = Eigen::Matrix<double,2,1>::Constant(0);
	};

	void init(double scale)
	{
		Eigen::Matrix<double, 3, 1> p,v,b_w,b_a,p_ic,g,w_m,a_m, p_ig, p_vw;
		Eigen::Quaternion<double> q, q_ci,q_wv,q_mi;
		Eigen::Matrix<double,nState_,nState_> P;

		g << 0, 0, 9.81;
		b_w << 0,0,0;
		b_a << 0,0,0;

//		Eigen::VectorXd data = handlers[Sensors::VISION]->init();

		v << 0,0,0;
		w_m << 0,0,0;
		a_m =g;	// zero vel initialization....

		p_ic = Eigen::Matrix<double,3,1>(0, 0, 0);
		q_ci=Eigen::Quaternion<double>(0, 1, -1, 0);	//180° about x, then 90° about z
		q_ci.normalize();
		q_wv=Eigen::Quaternion<double>(1, 0, 0, -1);
		q_wv.normalize();
		q_mi=Eigen::Quaternion<double>(0, 1, 0, 0);
		q_mi.normalize();
		p_ig = Eigen::Matrix<double,3,1>(0, 0, 0);
		p_vw = Eigen::Matrix<double,3,1>(0, 0, 0);
		double alpha=-1.1;//-3.14159/3; 	// about -63° in Switzerland...
		double beta=3.14159/2;		// asctec rotation things...


		P = Eigen::Matrix<double,nState_,nState_>::Constant(0);
		P << 0.001493068373518,0.000016795724241,0.000119921739281,0.002435606233256,0.000030453768416,0.000070820472574,-0.000003866554412,0.000029797478028,0.000045283148405,-0.000000365807920,-0.000000280823170,-0.000001150202272,0.000044507593556,-0.000026134667784,-0.000002272987497,0.000075073653325,0.000014214207277,0.000935768747675,-0.000206776016191,-0.000004778516809,-0.000003591769853,0.000001420108650,-0.000036692145303,0.000022720346108,0.000005383545970,0.000003548926076,-0.000000438548244,0.000001605617315,-0.000042247606531,0.000025051839152,0.000003548926071,-0.000003591769861,0.000001420108650,-0.000001258125512,0.000003892161322,-0.000004778516808
				,0.000036405351629,0.001485605733093,0.000157515378017,0.000134528519106,0.002213473005599,-0.000002326828273,0.000049848830966,-0.000024523912240,-0.000018092258275,0.000000265348029,-0.000000388112453,-0.000000153906732,0.000026666854798,0.000036995973040,-0.000003614940887,0.000127575874284,-0.000956182109613,0.000003851841130,0.000114120987639,0.000002115678455,-0.000002900053648,-0.000001084017640,-0.000029728075591,-0.000034752449282,0.000002690735217,-0.000001226506893,-0.000000760928560,-0.000000180847518,-0.000032304840500,-0.000035574799833,-0.000001226506895,-0.000002900053646,-0.000001084017644,-0.000000099863618,-0.000003457897994,0.000002115678458
				,0.000124013135756,0.000151022432812,0.002117363491120,0.000300469523412,-0.000077375015879,0.002453193392368,-0.000004461790053,0.000027164204599,0.000016688032997,-0.000000052572252,-0.000000250265894,-0.000000021042270,0.000007718187371,-0.000004957786669,-0.000024785179036,0.000668587003660,0.000164075997936,-0.000078465879494,-0.000018514163559,0.000001322434279,-0.000000279935460,-0.000000305881979,-0.000002217097921,0.000009785781015,-0.000007635062360,-0.000009365472055,0.000002523990775,0.000002154015792,-0.000002902958348,0.000010072535537,-0.000009365472066,-0.000000279935448,-0.000000305881953,-0.000000291114979,-0.000005660878428,0.000001322434271
				,0.002272069749682,-0.000100596657712,0.000277764144778,0.012975953463447,-0.000285316239225,0.000203560989685,0.000291033168210,0.000921901654628,-0.000254753888261,-0.000004188785166,-0.000009685265265,-0.000000103439105,-0.000002405026933,0.000039077727578,0.000001546540809,0.000155629511967,0.000128897337182,0.000794246011895,-0.000018858102191,0.000003718188339,-0.000001223763602,0.000001704693959,-0.000008449050659,-0.000020693982562,0.000005580299719,0.000003055176164,-0.000001514762698,0.000004222814373,-0.000006808177350,-0.000021676439620,0.000003055176141,-0.000001223763600,0.000001704693950,0.000000153945285,-0.000002846074745,0.000003718188338
				,-0.000028016344506,0.002383892573937,-0.000060769927527,0.000124091682822,0.012026719150291,-0.001698789121583,-0.000333753520886,0.000193940711854,0.000020471284482,0.000004427573708,-0.000004065005187,-0.000000226481480,-0.000042048659382,0.000008209745754,-0.000010146812125,0.000014652590318,-0.000958555077891,0.000072345934717,0.000325352136870,0.000001192032830,-0.000001025298317,-0.000000739632044,0.000023864584448,-0.000011579095297,-0.000001425919291,-0.000003544198467,-0.000003031809898,0.000004289847938,0.000023432349298,-0.000011166686654,-0.000003544198457,-0.000001025298319,-0.000000739632046,-0.000000710207293,-0.000005752195489,0.000001192032832
				,0.000123492481537,-0.000057393954497,0.002453022233726,0.000459537490942,-0.001947006692771,0.006104507562119,0.000102148020040,-0.000007932549666,-0.000006745737025,-0.000001183833381,0.000000410873480,0.000000220109209,-0.000004510101270,-0.000000787187697,-0.000061375903441,0.000731177816230,0.000178229073579,-0.000116076955020,-0.000055867141422,0.000000736593867,0.000001008991139,0.000000769738087,-0.000005103059422,0.000007356839996,0.000002662001048,0.000002014172105,0.000000239088554,-0.000001252600563,-0.000004368402179,0.000006787290030,0.000002014172105,0.000001008991136,0.000000769738088,0.000001402219614,0.000002211890073,0.000000736593867
				,0.000010165012606,-0.000033359501628,-0.000008016143591,0.000354674322436,-0.000770747609272,0.000058016969898,0.000343452260060,0.000045190528649,0.000070604593642,-0.000002125684810,-0.000000596189013,-0.000000812808033,0.000001416857132,-0.000022175058770,-0.000000296476642,0.000040423556355,-0.000056992673599,-0.000140543317752,-0.000096513047005,-0.000001241488617,0.000000254815011,-0.000000113081618,-0.000001440336451,0.000005101501475,-0.000000106301008,0.000000772794016,0.000000901606122,-0.000001114618571,-0.000001754065947,0.000005192801982,0.000000772794017,0.000000254815010,-0.000000113081615,0.000000017805144,0.000002161365293,-0.000001241488617
				,-0.000044694044311,-0.000043105075589,0.000026837995184,0.000559900900136,0.000137099521037,-0.000016683747962,0.000043047465325,0.000353127840422,0.000065197764757,-0.000000420628566,-0.000003272102163,-0.000000111975197,0.000020371938625,-0.000000021105899,-0.000001916346397,-0.000010952232701,0.000079459465184,-0.000148475740711,-0.000004416466656,0.000000305688216,-0.000000735674393,0.000000367367588,-0.000004056898804,-0.000001274979884,0.000000447079958,-0.000000085702228,-0.000000721694069,0.000002106279440,-0.000004318457595,-0.000001473835657,-0.000000085702235,-0.000000735674392,0.000000367367586,-0.000000167644318,-0.000000988484351,0.000000305688216
				,0.000038397773795,-0.000015997897932,0.000020325740256,-0.000240286503065,0.000055845236076,-0.000012802087587,0.000063293751417,0.000074033010565,0.000420563968989,-0.000000398647649,-0.000000308714246,-0.000003874752576,0.000004135751301,-0.000000889095305,-0.000000099247476,-0.000011397588742,0.000054961475536,-0.000172010888699,-0.000290930727744,-0.000002951259794,-0.000000215074212,-0.000001445604775,0.000000761130187,-0.000000248445866,-0.000002504366038,-0.000001658953182,0.000000207660529,-0.000003969931646,-0.000001498863996,0.000000155706546,-0.000001658953157,-0.000000215074217,-0.000001445604768,-0.000000567294129,0.000000535485367,-0.000002951259791
				,-0.000000485141409,0.000000358204754,0.000000003372545,-0.000004774819277,0.000007849920002,-0.000002273228440,-0.000002998135552,-0.000000008005024,-0.000000370169570,0.000001817178259,0.000000001669835,-0.000000027893561,0.000000327495091,0.000000583530959,0.000000009316723,-0.000000419115630,0.000000539119703,0.000000748103681,0.000000802297112,-0.000000085137614,0.000000028788491,-0.000000010196774,-0.000000089722997,0.000000132031227,-0.000000059913838,0.000000014342813,0.000000025597965,0.000000038641419,-0.000000082340215,0.000000159428116,0.000000014342813,0.000000028788491,-0.000000010196774,0.000000002553817,0.000000006999426,-0.000000085137614
				,-0.000000071773024,-0.000000379792715,-0.000000035848650,-0.000006517919796,-0.000003519716840,0.000000484190463,-0.000000241589586,-0.000002371139103,-0.000000087281763,-0.000000036737960,0.000001870417996,-0.000000056243452,-0.000000174214309,0.000000764214580,0.000000066238826,0.000000012521630,-0.000000144922140,0.000000548187670,-0.000000302511533,-0.000000066048656,-0.000000064757801,0.000000007454061,-0.000000088910826,0.000000088803768,-0.000000048561941,-0.000000079018515,-0.000000053826187,0.000000129124309,-0.000000194566231,0.000000110414713,-0.000000079018516,-0.000000064757801,0.000000007454061,-0.000000048025242,-0.000000087207218,-0.000000066048656
				,-0.000001023621566,-0.000000030653925,0.000000123736200,0.000001154700459,-0.000000112234527,0.000000325830142,-0.000000639521090,0.000000127432261,-0.000003751557961,-0.000000014624391,-0.000000003970271,0.000001790821131,-0.000000041559815,0.000000717232113,0.000000080726109,0.000000080660080,-0.000000235101117,0.000000986375331,0.000002676000736,-0.000000010131034,-0.000000019622414,-0.000000048328508,0.000000065019449,0.000000070765076,-0.000000077683504,-0.000000085267592,-0.000000002582259,-0.000000070902825,0.000000032544232,0.000000082759266,-0.000000085267592,-0.000000019622414,-0.000000048328508,-0.000000005818903,-0.000000072684286,-0.000000010131034
				,0.000045893720615,0.000025523238836,0.000007749283624,0.000010432953800,-0.000054454198101,-0.000001570870561,0.000005889109069,0.000021745385819,0.000005369469724,0.000000201860835,-0.000000256082482,-0.000000008872147,0.000282139271899,-0.000007079306573,-0.000012251684833,-0.000001033383171,0.000004563453643,-0.000013785601918,-0.000003945455892,-0.000002934623184,-0.000005112057916,0.000002094868814,-0.000061099098418,0.000000805317952,0.000005308173892,0.000002192420707,-0.000002175130868,0.000007197154137,-0.000068154241170,0.000001711892479,0.000002192420682,-0.000005112057918,0.000002094868808,-0.000001953540145,0.000001263656870,-0.000002934623184
				,-0.000025410568629,0.000036129726901,-0.000005181874137,0.000035734828174,0.000000893275850,0.000001076970758,-0.000021261600621,-0.000001613492030,-0.000001077283099,0.000001363628078,0.000000149637356,0.000000720209155,-0.000006543520646,0.000272420463666,0.000002927607172,0.000000116216314,0.000009621162422,0.000006377064931,0.000001716246977,0.000007615850555,-0.000002515640496,0.000000091326655,0.000001867247554,-0.000057004630694,0.000003821825635,-0.000001475111016,-0.000002068077813,-0.000004526447280,0.000001281052271,-0.000060968145960,-0.000001475110998,-0.000002515640495,0.000000091326648,-0.000001347538770,-0.000013582139123,0.000007615850561
				,-0.000002602451212,-0.000003785702903,-0.000025016240237,-0.000002074670501,-0.000008421009860,-0.000062243236733,-0.000001332277372,-0.000002583446244,-0.000000245050158,0.000000040602634,0.000000034188061,0.000000103451136,-0.000012689027105,0.000003083690081,0.000081098735595,-0.000007597059725,-0.000001263646457,0.000002567949684,0.000000490367477,0.000000298823942,0.000000335000328,-0.000000244141196,0.000002241146184,-0.000000418273649,-0.000001873570636,-0.000001496404195,0.000000682023055,0.000000041344287,0.000002880792637,-0.000000261147054,-0.000001496404196,0.000000335000330,-0.000000244141193,0.000000532115193,-0.000001191567907,0.000000298823941
				,0.000083101834638,0.000114321891834,0.000668052643517,0.000197943168132,-0.000044283738589,0.000725946126431,0.000043751970120,-0.000008711095149,-0.000010873950878,-0.000000290406907,-0.000000018339403,0.000000033073615,-0.000001506355535,-0.000000026523209,-0.000007423160847,0.000394871229604,-0.000003658497485,0.000001342899876,-0.000004436933579,0.000000550980561,-0.000000179038819,0.000000082128657,0.000001203118811,-0.000000083669864,0.000001441744721,-0.000000227737955,0.000000068849501,-0.000000327620333,0.000001657602892,0.000000028969041,-0.000000227737953,-0.000000179038819,0.000000082128658,0.000001713846900,0.000000035857314,0.000000550980561
				,-0.000011073313378,-0.000939423763213,0.000160963634196,-0.000038237204214,-0.000807483809031,0.000155826421706,-0.000096368435048,0.000070365208261,0.000051845566311,0.000000233910694,-0.000000243982024,-0.000000208295133,0.000005041438000,0.000010490979860,-0.000001506471661,-0.000009770460849,-0.000001068542489,0.000005005944657,0.000000359894120,-0.000001590385939,0.000002725788873,-0.000002084520904,0.000054536983090,0.000001070079476,-0.000000735140433,0.000000203058526,-0.000000131917238,0.000000054733227,0.000051018512415,0.000001349008215,-0.000001602411901,-0.000000965848873,-0.000000948167199,-0.000002657940018,-0.000000861818501,-0.000000318391420
				,0.000951894881904,0.000023466262807,-0.000080721786458,0.000917635886396,0.000197160980696,-0.000125806022721,-0.000143366847525,-0.000116597020038,-0.000166476285801,0.000000742362194,0.000000999763328,0.000001090536210,-0.000012224009150,0.000006116650071,0.000002083823893,-0.000000776248086,-0.000004452728049,-0.000001254641317,-0.000000413358926,-0.000003045591728,0.000000044750833,0.000000152306923,0.000001612105440,0.000047698793515,-0.000002172268859,0.000001164016107,0.000000321300208,0.000002454604799,0.000001870360583,0.000045749653351,-0.000000351058848,0.000000616461101,-0.000000080470232,0.000000389000318,0.000003878917576,-0.000000958766632
				,-0.000212021088931,0.000112284960912,-0.000021684130085,-0.000033875392875,0.000295917054038,-0.000052099968082,-0.000089181841018,-0.000004961657902,-0.000295021661029,0.000000614896473,-0.000000299084307,0.000002717756851,-0.000002417226704,0.000002319162040,0.000000376270409,-0.000004307273833,-0.000002735785034,0.000000158670213,0.000001090383097,0.000000567959008,0.000000184201668,-0.000001902575044,-0.000000526146783,-0.000002100392049,0.000015349204402,0.000001316295122,-0.000000573061825,0.000004033144500,-0.000003528515276,-0.000001556224109,0.000008146274868,-0.000002680532919,0.000001479474597,-0.000003304162692,0.000005708791379,0.000000759490040
				,-0.000004730817364,0.000002140123997,0.000001328389157,0.000003711295156,0.000001384510955,0.000000746320675,-0.000001197394083,0.000000306608208,-0.000002908598698,-0.000000226065984,-0.000000040881881,-0.000000003223008,-0.000002873666875,0.000007382738977,0.000000281126343,0.000000563873698,-0.000001629630712,-0.000003031207623,0.000000504925280,0.000006665982235,0.000000075745014,0.000000736119398,-0.000000295700717,-0.000000958559657,0.000000761530203,-0.000001574249725,-0.000000588105598,-0.000002737486187,0.000001358625467,-0.000002811879197,-0.000001574249717,0.000000075745016,0.000000736119391,-0.000000226151526,-0.000000138584248,0.000006665982236
				,-0.000003639813717,-0.000002942567290,-0.000000307858952,-0.000001108821604,-0.000000907226994,0.000000956960121,0.000000223009610,-0.000000695898929,-0.000000228788219,0.000000064834756,-0.000000049867347,-0.000000015644610,-0.000005035753190,-0.000002415513940,0.000000328703350,-0.000000190035194,0.000002754335864,0.000000040866334,0.000000207702022,0.000000079724871,0.000004219881009,-0.000000375438612,-0.000000919355203,0.000000608530417,-0.000002680126914,0.000000523227857,0.000000284953832,-0.000000127159501,0.000003149076404,0.000000297124322,0.000000523227856,0.000004219881009,-0.000000375438610,0.000002110024084,0.000000354469419,0.000000079724870
				,0.000001521390812,-0.000001077294767,-0.000000244287093,0.000001966697852,-0.000000802885108,0.000000838861524,-0.000000057425339,0.000000376951029,-0.000001427063634,-0.000000005360632,0.000000011001565,-0.000000037836393,0.000002139781996,-0.000000050033663,-0.000000245874567,0.000000108562960,-0.000002116504144,0.000000177668310,-0.000001901710055,0.000000703023480,-0.000000374664419,0.000003616577748,-0.000000938896707,-0.000000038201237,0.000001466737786,0.000000541063527,0.000000059152882,0.000000862191746,-0.000001173826860,-0.000000121264528,0.000000541063507,-0.000000374664410,0.000003616577733,-0.000000861545082,-0.000000105462199,0.000000703023478
				,-0.000011073313378,-0.000939423763213,0.000160963634196,-0.000038237204214,-0.000807483809031,0.000155826421706,-0.000096368435048,0.000070365208261,0.000051845566311,0.000000233910694,-0.000000243982024,-0.000000208295133,0.000005041438000,0.000010490979860,-0.000001506471661,-0.000009770460849,0.001050174294002,-0.000002946456347,0.000030333413265,-0.000001590385939,0.000002725788873,-0.000002084520904,-0.000000941078929,-0.000004694528448,-0.000002664062973,0.000000203058526,-0.000000131917238,0.000000054733227,0.000003131064851,-0.000002621034749,0.000000203058519,0.000002725788876,-0.000002084520913,0.000002652494553,-0.000000525984653,-0.000001590385940
				,0.000951894881904,0.000023466262807,-0.000080721786458,0.000917635886396,0.000197160980696,-0.000125806022721,-0.000143366847525,-0.000116597020038,-0.000166476285801,0.000000742362194,0.000000999763328,0.000001090536210,-0.000012224009150,0.000006116650071,0.000002083823893,-0.000000776248086,-0.000006423824503,0.001159217718649,0.000191258451188,-0.000003045591728,0.000000044750833,0.000000152306923,0.000004627174636,-0.000001323836680,0.000000275997902,0.000001164016107,0.000000321300208,0.000002454604799,0.000001550624319,0.000001584157579,0.000001164016098,0.000000044750832,0.000000152306924,-0.000001961489626,-0.000000667992057,-0.000003045591728
				,-0.000212021088931,0.000112284960912,-0.000021684130085,-0.000033875392875,0.000295917054038,-0.000052099968082,-0.000089181841018,-0.000004961657902,-0.000295021661029,0.000000614896473,-0.000000299084307,0.000002717756851,-0.000002417226704,0.000002319162040,0.000000376270409,-0.000004307273833,0.000027502872172,0.000188786044988,0.001778122498746,0.000000567959008,0.000000184201668,-0.000001902575044,0.000000239060646,-0.000000408234559,0.000001208557342,0.000001316295122,-0.000000573061825,0.000004033144500,0.000001952120447,-0.000000117543597,0.000001316295114,0.000000184201665,-0.000001902575037,0.000001168053376,-0.000000776803477,0.000000567959006
				,0.000003543442496,-0.000001395632496,-0.000009294117979,0.000004285311663,-0.000004311034582,0.000002313709925,0.000001130585394,0.000000014016213,-0.000001580996819,0.000000061632949,-0.000000052115155,-0.000000045957886,0.000002246493478,-0.000001518198507,-0.000001496986015,-0.000000154283819,0.000000148355482,0.000001051760752,0.000001244837326,-0.000001578833912,0.000000515931284,0.000000534548305,-0.000001371524672,-0.000000320765422,0.000008157104777,0.000009874674800,-0.000002745093531,-0.000001909147630,-0.000000916711725,-0.000000072976776,0.000009874674811,0.000000515931294,0.000000534548301,0.000000503641097,0.000006674731259,-0.000001578833912
				,-0.000000445968572,-0.000000889185016,0.000002563340256,-0.000000997156770,-0.000003813415245,0.000000391592171,0.000001068519708,-0.000000686638771,0.000000162517515,0.000000055202998,-0.000000031812651,0.000000015154781,-0.000001829143286,-0.000002064720577,0.000000630944780,0.000000102508564,-0.000000096905167,0.000000270967375,-0.000000561833654,-0.000000577436755,0.000000269314747,0.000000053624598,0.000000357552441,0.000000580558987,-0.000002957279569,-0.000002769029072,0.000016401528797,0.000003360259003,0.000000467615616,0.000000752285693,-0.000002769029071,0.000000269314745,0.000000053624595,0.000000036345380,-0.000012890589499,-0.000000577436754
				,0.000001574701150,-0.000000085658740,0.000002119635117,0.000003221125099,0.000004997545024,-0.000001487351027,-0.000001488439633,0.000002004124244,-0.000004092480697,0.000000145375832,0.000000074287773,-0.000000056037573,0.000006999928641,-0.000004507532213,0.000000030507133,-0.000000386170229,0.000000092119795,0.000002575497909,0.000004172773217,-0.000002762709499,-0.000000119343413,0.000000835455914,-0.000001597137636,0.000000990799174,-0.000002489228033,-0.000001900976490,0.000003362918557,0.000012322713634,-0.000002293478188,0.000001670496660,-0.000001900976474,-0.000000119343408,0.000000835455902,-0.000000163786524,-0.000006923461528,-0.000002762709496
				,-0.000042803606054,-0.000031469626036,-0.000002727018341,-0.000010284169722,0.000026053206744,-0.000004726050296,-0.000002578421080,-0.000004938475894,-0.000001489648854,-0.000000032545082,-0.000000044442415,0.000000046807745,-0.000068254651581,0.000001355585859,0.000002738832524,0.000001622606452,0.000051077539080,0.000001323920327,-0.000003742397867,0.000001331157202,0.000003100935640,-0.000001181366285,0.000003079670745,0.000001891677706,0.000002014677655,-0.000001136279209,0.000000447710739,-0.000002186243650,-0.000000456238115,0.000001331157201,0.000003100935645,-0.000001181366292,0.000003037127238,0.000058609889226,0.000000344416055,-0.000001136279205
				,0.000024549358392,-0.000035821868065,0.000009972386514,-0.000022347814114,-0.000008740603494,0.000006029899335,0.000004666430155,-0.000001251557049,0.000000237766344,0.000000143613989,-0.000000016208116,0.000000100511884,0.000001602320185,-0.000061671801060,-0.000000233460662,-0.000000084083390,0.000001866380039,0.000045764955483,-0.000001617680209,-0.000002800199122,0.000000306981784,-0.000000167808979,-0.000002392135325,0.000001638099907,-0.000000063229207,-0.000000095438105,0.000000777466559,0.000001618846539,0.000004090457572,-0.000002800199125,0.000000306981783,-0.000000167808975,-0.000000611029005,0.000000870648490,0.000050115127242,-0.000000095438111
				,0.000003543442510,-0.000001395632507,-0.000009294117975,0.000004285311669,-0.000004311034587,0.000002313709929,0.000001130585394,0.000000014016214,-0.000001580996831,0.000000061632950,-0.000000052115155,-0.000000045957886,0.000002246493472,-0.000001518198531,-0.000001496986015,-0.000000154283820,-0.000001371524679,-0.000000320765411,0.000008157104780,-0.000001578833911,0.000000515931295,0.000000534548320,0.000000148355490,0.000001051760748,0.000001244837324,0.000009874674800,-0.000002745093529,-0.000001909147628,0.000006674731259,-0.000001578833904,0.000000515931281,0.000000534548295,0.000000503641093,-0.000000916711730,-0.000000072976761,0.000009874674811
				,-0.000003639813705,-0.000002942567285,-0.000000307858949,-0.000001108821599,-0.000000907227003,0.000000956960122,0.000000223009610,-0.000000695898926,-0.000000228788224,0.000000064834756,-0.000000049867347,-0.000000015644610,-0.000005035753171,-0.000002415513948,0.000000328703350,-0.000000190035195,-0.000000919355217,0.000000608530422,-0.000002680126917,0.000000079724872,0.000004219881008,-0.000000375438604,0.000002754335866,0.000000040866334,0.000000207702021,0.000000523227858,0.000000284953832,-0.000000127159500,0.000000354469420,0.000000079724873,0.000004219881010,-0.000000375438614,0.000002110024083,0.000003149076391,0.000000297124327,0.000000523227854
				,0.000001521390826,-0.000001077294766,-0.000000244287107,0.000001966697846,-0.000000802885104,0.000000838861528,-0.000000057425338,0.000000376951027,-0.000001427063623,-0.000000005360632,0.000000011001565,-0.000000037836392,0.000002139781996,-0.000000050033660,-0.000000245874570,0.000000108562960,-0.000000938896708,-0.000000038201243,0.000001466737808,0.000000703023478,-0.000000374664415,0.000003616577732,-0.000002116504150,0.000000177668307,-0.000001901710052,0.000000541063516,0.000000059152885,0.000000862191743,-0.000000105462204,0.000000703023478,-0.000000374664421,0.000003616577746,-0.000000861545088,-0.000001173826876,-0.000000121264530,0.000000541063524
				,-0.000001279668969,-0.000000158186400,-0.000000352762841,0.000000096501201,-0.000000212068153,0.000001143918503,-0.000000134242826,-0.000000040628927,-0.000000531425957,0.000000013853079,-0.000000037895029,-0.000000007623849,-0.000001823325273,-0.000001292926978,0.000000538249815,0.000001666515768,-0.000002665982747,0.000000378249058,-0.000003285803646,-0.000000233789782,0.000002104276027,-0.000000868741590,0.000002770944818,-0.000001971839182,0.000001168037074,0.000000524186626,0.000000028175666,-0.000000184732988,0.000000383470349,-0.000000233789782,0.000002104276029,-0.000000868741598,0.000009204677992,0.000003016304764,-0.000000616748114,0.000000524186623
				,0.000003958278879,-0.000003572391304,-0.000005597895949,-0.000002001246053,-0.000005460519990,0.000002183129208,0.000002160588686,-0.000000830683014,0.000000680291072,0.000000267190181,-0.000000040481154,-0.000000090296975,0.000001133044129,-0.000013261161291,-0.000001137077003,0.000000033840538,-0.000000761762867,0.000003942296139,0.000005690376003,-0.000000153562145,0.000000372658769,-0.000000108283716,-0.000000464426841,-0.000000711759126,-0.000000828641760,0.000006675859360,-0.000012862954267,-0.000006922993301,0.000017676330220,-0.000000153562147,0.000000372658772,-0.000000108283711,0.000000364476377,-0.000000344394610,0.000004157099942,0.000006675859355
				,-0.000004730817370,0.000002140123999,0.000001328389198,0.000003711295143,0.000001384510949,0.000000746320673,-0.000001197394081,0.000000306608206,-0.000002908598692,-0.000000226065984,-0.000000040881881,-0.000000003223008,-0.000002873666877,0.000007382738979,0.000000281126349,0.000000563873699,-0.000000295700712,-0.000000958559658,0.000000761530171,0.000006665982234,0.000000075745014,0.000000736119390,-0.000001629630718,-0.000003031207625,0.000000504925280,-0.000001574249713,-0.000000588105602,-0.000002737486190,-0.000000138584240,0.000006665982227,0.000000075745008,0.000000736119399,-0.000000226151535,0.000001358625460,-0.000002811879195,-0.000001574249758;


		if(p_vc_.norm()==0)
			ROS_WARN_STREAM("No position measurements received yet from camera");
		if((q_cv_.norm()==1) & (q_cv_.w()==1))
			ROS_WARN_STREAM("No attitude measurements received yet from camera");
		if(p_wg_.norm()==0)
			ROS_WARN_STREAM("No position measurements received yet from gps");



		q = Eigen::Quaternion<double>(1, 0, 0, 1);
		q.normalize();
//		if(((q_cv_.norm()==1) & (q_cv_.w()==1)) | (p_wg_.norm()!=0))	//if we have gps, we need to initialize q northwards!
//		{
//			q = Eigen::Quaternion<double>(1, 0, 0, 1);
//			q.normalize();
//		}
//		else
//		{
//			q = ( q_ci * q_cv_ *q_wv).conjugate();
//		}

		q_wv = q_cv_.conjugate()*q_ci.conjugate()*q.conjugate();

		if(p_wg_.norm()!=0)
		{
			 if(p_vc_[2]*p_wg_[2]!=0)
				 scale = fabs(p_vc_(2)/p_wg_(2));
			 p = p_wg_ - q.toRotationMatrix()*p_ig;
			 p_vw = p_vc_/scale - q_cv_.conjugate().toRotationMatrix()*q_ci.conjugate().toRotationMatrix()*p_ic - q_wv.conjugate().toRotationMatrix()*p;
		}
		else	// no gps measurements received
			p = q_wv.conjugate().toRotationMatrix()*p_vc_/scale - q.toRotationMatrix()*p_ic ;

		// adjust certainities according to guessed states...
		Eigen::Matrix<double,3,3> Pincr1 = (Eigen::Matrix<double,3,1>::Constant(1)).asDiagonal(); // q_vw
		Eigen::Matrix<double,3,3> Pincr2 = (Eigen::Matrix<double,3,1>::Constant((p_wg_-p_vc_/scale).norm())).asDiagonal(); // p_vw
		P(15,15)+=scale/2;
		P.block(16,16,3,3)+=Pincr1;	//q_vw
		P.block(31,31,3,3)+=Pincr2;	//p_vw


		poseFilter_.initialize(p,v,q,b_w,b_a,scale,q_wv,P,w_m,a_m,g,q_ci,p_ic,q_mi,p_ig,p_vw,alpha,beta);

		ROS_INFO_STREAM("filter initialized to: \n" <<
						"position: [" << p[0] << ", " << p[1] << ", " << p[2] << "]" <<
						"attitude (w,x,y,z): [" << q.w() << ", " << q.x() << ", " << q.y() << ", " << q.z() << "]");
		};


	void Config(sensor_fusion_core::Sensor_Fusion_CoreConfig& config, uint32_t level){

		if(level & sensor_fusion_core::Sensor_Fusion_Core_SET_HEIGHT){
			if(p_vc_.norm()==0)
			{
				ROS_WARN_STREAM("No measurements received yet to initialize position - using scale factor " << config.scale_init << " for init");
				init(config.scale_init);
			}
			else
				init(p_vc_[2]/config.set_height);
			config.set_height = false;
		}
	}
};

#endif /* VISMAGGPS_MEASUREMENTS_H_ */

