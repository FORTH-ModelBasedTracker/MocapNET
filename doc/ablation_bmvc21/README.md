## Towards Holistic Real-time Human 3D Pose Estimation using MocapNETs

### Ablation study experimental results

_Ammar Qammaz_
<br/>
<br/>

This is a summary of the ablation experiments that accompanies the following paper:

A. Qammaz and A.A. Argyros, "Towards Holistic Real-time Human 3D Pose Estimation using MocapNETs", In British Machine Vision Conference (BMVC 2021), (to appear), BMVA, UK, November 2021. 

 We conducted all the experiments listed here by acquiring the STB and RHD datasets from the [VAE 3D hands repository](https://github.com/spurra/vae-hands-3d). We used the [HandOnlyTest utility](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/MocapNET2/HandOnlyTest/handTest.cpp) to compute standalone 3D hand poses and this [Python utility](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/src/python/compareUtility/compareUtility.py) to compare the 3D output of the proposed method to ground truth and extract results. 

Due to RAM constraints, the largest generated data samples consisted of 7.276.716 M samples. A fully trained ensemble for the left hand took approximately 6 days to train for 7M samples. Using a tenth of the sample size (727K)  we where able to conduct faster experiments to cover a larger variety of combinations, but we also used an intermediate 1/2 sample ratio with 3,638,568 samples also comparing the possible variance that sample size introduces to the training procedure. It should be noted that we generate each of the [3 differently sized datasets *once* using this tool](https://github.com/FORTH-ModelBasedTracker/MocapNET/blob/master/scripts/createRandomHandDataset.sh#L58) in order for all the experiments across a sample size to be directly compareable. The tensorflow [randomized seed](https://www.tensorflow.org/api_docs/python/tf/random/set_seed) is also fixed to 0 for each training to be deterministic and reproducible.

The tables below are all variations on the final proposed setup of the MocapNET3 ensemble that uses *Hierarchical Coordinate Descent (HCD)* optimization as a fine tuning step, *eNSRM* descriptors to encode 2D input data, *SWISH* activations, a dropout of *0.3* or 30% and where each of the encoders regresses *1 degree of freedom(d.o.f.)* . Starting from this configuration we alternate settings to inspect how they affect 3D pose estimation training accuracy on the STB and RHD datasets. 


The first set of "small" sample size experiments using 1/10 of the final sample size for different combinations of a 2D Descriptor, comparing the proposed eNSRM with the rich diagonal with the [MocapNET2 NSRM](http://users.ics.forth.gr/~argyros/mypapers/2021_01_ICPR_Qammaz.pdf) and the [Self-normalizing (SELU)](https://arxiv.org/abs/1706.02515) activations of [MocapNET 1](http://users.ics.forth.gr/~argyros/mypapers/2019_09_BMVC_mocapnet.pdf) and [MocapNET 2](http://users.ics.forth.gr/~argyros/mypapers/2021_01_ICPR_Qammaz.pdf). Finally, all runs are tested with and without the Hierarchical Coordinate Descent fine-tuning step introduced in [MocapNET 2](http://users.ics.forth.gr/~argyros/mypapers/2021_01_ICPR_Qammaz.pdf).

<br/>

| Sample Size | Dropout | D.o.f. | HCD | Descriptor | Activation |  STB avg. acc. | RHD avg. acc.  |
| :-:            | :-:        |         :-: |      :-:     |      :-:     |      :-:     |      :-:     |      :-:     |
| 727.591 | 0.3 | 1 | &#10003; | NSRM | SWISH | 10.2 mm | 24.9 mm |
| 727.591 | 0.3 | 1 | &#10003; | eNSRM | SWISH | 11.8 mm | 25.0 mm |
| 727.591 | 0.3 | 1 | &#10003; | eNSRM | SELU | 10.1 mm | 25.2 mm |
| 727.591 | 0.3 | 1 | &#10003; | NSRM | SELU | 10.6 mm | 25.1 mm |
| 727.591 | 0.3 | 1 | &#10007; | NSRM | SWISH | 15.6 mm | 26.1 mm |
| 727.591 | 0.3 | 1 | &#10007; | eNSRM | SWISH | 18.2 mm | 26.4 mm |
| 727.591 | 0.3 | 1 | &#10007; | eNSRM | SELU | 18.0 mm | 27.1 mm |
| 727.591 | 0.3 | 1 | &#10007; | NSRM | SELU | 15.9 mm | 26.5 mm |


One of the first observations is that the HCD step plays an important role in improving accuracy, especially on temporarilly cohesive input data such as the STB data.  The results also show that since the HCD improvements improve the results of the neural network, in order to properly compare neural network configurations this is best done when HCD is turned off. We also observe that for this small sample size although the proposed SWISH activation performs better than the [MNET2](http://users.ics.forth.gr/~argyros/mypapers/2021_01_ICPR_Qammaz.pdf) [SELU]((https://arxiv.org/abs/1706.02515) the richer diagonal actually makes the task more difficult for the neural network although the slightly increased error is still recoverable via the HCD step.
 
Another idea we tested is grouping together encoder outputs. Since the internal state of each of the encoders could theoretically be reused across more than one degrees of freedom, especially since many joints feature more than one degrees of freedoms, we attempted to regress 2,3 and 5 degrees of freedom using the same encoder. Testing these configurations without the HCD step reveals a gradually worse accuracy. This is to be expected since this was one of the reasons for the ensemble architecture of encoders with 1 d.o.f. . Our explanation for this behavior is that sharing the capacity of the network between more than one degrees of freedom means less network computations resources dedicated to each of the outputs. It also means that during training the loss function becomes less indicative of each of the outputs making the training procedure gradually harder. 

<br/>

| Sample Size | Dropout | D.o.f. | HCD | Descriptor | Activation |  STB avg. acc. | RHD avg. acc.  |
| :-:            | :-:        |         :-: |      :-:     |      :-:     |      :-:     |      :-:     |      :-:     |
| 727.591 | 0.3 | 2 | &#10007; | eNSRM | SWISH | 19.8 mm | 25.0 mm |
| 727.591 | 0.3 | 3 | &#10007; | eNSRM | SWISH | 25.5 mm | 24.2 mm |
| 727.591 | 0.3 | 5 | &#10007; | eNSRM | SWISH | 27.0 mm | 27.3 mm |

In order to successfully deepen a densely connected network we have employed residual connections and relatively intense dropout rates. Concerned about wether the high drop-out rate could cause the network encoders (seen in Figure 2 of the paper) to over-rely on the skip connections and thus effectively really utilize only a lower total number of layers, we performed experiments varying the dropout rate to study the effects. We observe that increased dropout improves results especially in the RHD dataset, however the accuracy variance is not indicative of such a side-effect. 

<br/>

| Sample Size | Dropout | D.o.f. | HCD | Descriptor | Activation |  STB avg. acc. | RHD avg. acc.  |
| :-:            | :-:        |         :-: |      :-:     |      :-:     |      :-:     |      :-:     |      :-:     |
| 727.591 | 0.1 | 1 | &#10003; | eNSRM | SWISH | 10.1 mm | 25.3 mm |
| 727.591 | 0.1 | 1 | &#10007; | eNSRM | SWISH | 19.1 mm | 27.2 mm | 
| 727.591 | 0.2 | 1 | &#10003; | eNSRM | SWISH | 10.9 mm | 24.9 mm |
| 727.591 | 0.2 | 1 | &#10007; | eNSRM | SWISH | 18.2 mm | 26.7 mm |
| 727.591 | 0.25 | 1 | &#10003; | eNSRM | SWISH | 10.4 mm | 25.0 mm |
| 727.591 | 0.25 | 1 | &#10007; | eNSRM | SWISH | 19.3 mm | 26.8 mm |


We also performed some ablation experiments using the full sample size proposed in the paper. The following experiments that took approximately 3 weeks to complete, show less variance between the different combinations of design characteristics when HCD is enabled.

<br/>

| Sample Size | Dropout | D.o.f. | HCD | Descriptor | Activation |  STB avg. acc. | RHD avg. acc.  |
| :-:            | :-:        |         :-: |      :-:     |      :-:     |      :-:     |      :-:     |      :-:     |
| 7.276.716 | 0.3 | 1 | &#10003; | eNSRM | SWISH | 9.9 mm | 25.3 mm |
| 7.276.716 | 0.3 | 1 | &#10007; | eNSRM | SWISH | 20.5 mm | 26.8 mm |
| 7.276.716 | 0.3 | 1 | &#10003; | NSRM | SWISH | 9.8 mm | 25.0 mm |
| 7.276.716 | 0.3 | 1 | &#10007; | NSRM | SELU | 19.5 mm | 26.3 mm |
| 7.276.716 | 0.3 | 1 | &#10003; | NSRM | SELU | 9.9 mm | 25.2 mm |
| 7.276.716 | 0.3 | 1 | &#10007; | NSRM | SELU | 17.6 mm | 27.5 mm |


Finally, using a dataset 1/2 the size of our final size we once again perform variations on the proposed configuration and we manage to slightly improve the best results of our method achieving 9.7 mm for the STB and 25.0 mm for the RHD. However, since the BMVC publication policies prohibit updating experimental results during the rebuttal period we did not include these slightly improved figures on the paper. 

<br/>

| Sample Size | Dropout | D.o.f. | HCD | Descriptor | Activation |  STB avg. acc. | RHD avg. acc.  |
| :-:            | :-:        |         :-: |      :-:     |      :-:     |      :-:     |      :-:     |      :-:     |
| 3.638.568 | 0.3 | 1 | &#10003; | eNSRM | SWISH | 9.7 mm | 25.0 mm |
| 3.638.568 | 0.3 | 1 | &#10007; | eNSRM | SWISH | 18.6 mm | 26.7 mm |
| 3.638.568 | 0.3 | 1 | &#10003; | NSRM | SWISH | 10.0 mm | 24.9 mm |
| 3.638.568 | 0.3 | 1 | &#10007; | NSRM | SWISH | 17.6 mm | 26.1 mm |
| 3.638.568 | 0.3 | 1 | &#10003; | NSRM | SELU | 11.5 mm | 25.1 mm |
| 3.638.568 | 0.3 | 1 | &#10007; | NSRM | SELU | 18.9 mm | 26.3 mm |
| 3.638.568 | 0.3 | 1 | &#10003; | eNSRM | SELU | 9.9 mm | 25.2 mm |
| 3.638.568 | 0.3 | 1 | &#10007; | eNSRM | SELU | 17.6 mm | 27.5 mm |
| 3.638.568 | 0.1 | 1 | &#10003; | eNSRM | SWISH | 10.2 mm | 25.2 mm |
| 3.638.568 | 0.1 | 1 | &#10007; | eNSRM | SWISH | 17.2 mm | 26.9 mm |
| 3.638.568 | 0.1 | 1 | &#10003; | eNSRM | SELU | 10.0 mm | 25.4 mm |
| 3.638.568 | 0.1 | 1 | &#10007; | eNSRM | SELU | 16.3 mm | 27.2 mm |
| 3.638.568 | 0.1 | 1 | &#10003; | NSRM | SELU | 10.8 mm | 25.2 mm |
| 3.638.568 | 0.1 | 1 | &#10007; | NSRM | SELU | 19.8 mm | 26.6 mm |


To summarize our ablation study results, the hierarchical coordinate descent (HCD) algorithm provides significant pose estimation improvements regardless of the configuration and training of the neural network providing the initial 3D pose estimation. That being said, a network that can arrive closer to the correct solution makes HCD perform even better. A second conclusion from our experimental observations is that a reduced dropout rate does not seem to have a dramatic effect on error thus suggesting that the utilized capacity of the network remains similar under all tested dropout ratios. We observe that a larger number of samples improves network accuracy especially in the case of eNSRM descriptors that feature more complex features and also perform 2D alignment of 2D shapes that are more difficult to "learn". Finally, SELU and SWISH activation functions seem to perform similarly, however due to the slight edge of SWISH experiments and their improved computational performance they seem to be a good choice for the MNET3 ensemble.

