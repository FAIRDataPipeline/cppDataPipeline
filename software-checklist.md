# SCRC Software checklist

This checklist has been developed by the Scottish Covid-19 Response Consortium and has been used to assess the software engineering aspects of "model readiness" for our 
six epidemiological models. This is intended to be part of a broader scheme to evaluate and describe suitability of model results to be used in decision making so 
this checklist covers only the software implementation and assumes that other documents cover questions about model validation, quality of science, 
data provenance and quality and policy readiness.

In order to use this checklist for the SCRC FAIR data pipeline components, three questions that apply only to software producing scientific results have been marked "models only" and for other software an N/A response can be given.

## Software details

### Model / software name

> cppDataPipeline

### Date

> 18/01/22

### Version identifier

> v0.2.0

## This assessment

### Filled in by

Name

> Ryan Field

Role on project: 

> RSE

Person identifier:

> https://github.com/RyanJField

### Overall statement

Do we have sufficient confidence in the correctness of the software to trust the results?

This is your overall judgement on the level of confidence based on all the aspects of the checklist. There is no formulaic way to arrive at this overall assessment based on the individual checklist answers but please explain how the measures in place combine to reach this level of confidence and make clear any caveats (eg applies for certain ways of using the software and not others).

> - [ ] Yes
> - [x] Yes, with caveats
> - [ ] No
>
> The code is well written and tested through unit tests and continous integration, the documentation is still a work in progress.

## Checklist

Please use a statement from this list: "Sufficiently addressed", "Some work remaining or caveats", or "Needs to be addressed" to begin each response.

Additionally, for each question please explain the situation and include any relevant links (eg tool dashboards, documentation). The sub bullet points are to make the scope of the question clear and should be covered if relevant but do not have to be answered individually.

### Can a run be repeated and reproduce exactly the same results? (models only)

- How is stochasticity handled?
- Is sufficient meta-data logged to enable a run to be reproduced: Is the exact code version recorded (and whether the repository was "clean"), including versions of dependent libraries (e.g. an environment.yml file or similar) along with all command line arguments and the content of any configuration files? 
- Is there up-to-date documentation which explains precisely how to run the code to reproduce existing results? 

> - [ ] Sufficiently addressed
> - [ ] Some work remaining or caveats
> - [ ] Needs to be addressed
> - [x] N/A

### Are there appropriate tests?  (And are they automated?)

- Are there unit tests? What is covered?
- System and integration tests?  Automated model validation tests?
- Regression tests? (Which show whether changes to the code lead to changes in the output. Changes to the model will be expected to change the output, but many other changes, such as refactoring and adding new features, should not. Having these tests gives confidence that the code hasn't developed bugs due to unintentional changes.)
- Is there CI?
- Is everything you need to run the tests (including documentation) in the repository (or the data pipeline where appropriate)?

> - [ ] Sufficiently addressed
> - [x] Some work remaining or caveats
> - [ ] Needs to be addressed
> 
> Tests are run via GitHub Actions CI ([![software-checklist][https://img.shields.io/badge/software-checklist-3891A6]][https://github.com/FAIRDataPipeline/cppDataPipeline/blob/main/software-checklist.md]) with good code
> coverage. 

### Are the scientific results of runs robust to different ways of running the code? (models only)

- Running on a different machine?
- With different number of processes?
- With different compilers and optimisation levels?
- Running in debug mode?

(We don't require bitwise identical results here, but the broad conclusions after looking at the results of the test case should be the same.) 

> - [ ] Sufficiently addressed
> - [ ] Some work remaining or caveats
> - [ ] Needs to be addressed
> - [x] N/A

### Has any sort of automated code checking been applied?

- For C++, this might just be the compiler output when run with "all warnings". It could also be more extensive static analysis. For other languages, it could be e.g. pylint, StaticLint.jl, etc.
- If there are possible issues reported by such a tool, have they all been either fixed or understood to not be important?

> - [ ] Sufficiently addressed
> - [x] Some work remaining or caveats
> - [ ] Needs to be addressed
> 
> Minimal warnings are present with some notes.

### Is the code clean, generally understandable and readable and written according to good software engineering principles?

- Is it modular?  Are the internal implementation details of one module hidden from other modules?
- Commented where necessary?
- Avoiding red flags such as very long functions, global variables, copy and pasted code, etc.?

> - [ ] Sufficiently addressed
> - [x] Some work remaining or caveats
> - [ ] Needs to be addressed
> 
> The code is organised and follows best practice but requires some refactoring to remove redundant code.

### Is there sufficient documentation?

- Is there a readme?
- Does the code have user documentation?
- Does the code have developer documentation?
- Does the code have algorithm documentation? e.g. something that describes how the model is actually simulated, or inference is performed?
- Is all the documentation up to date? 

> - [ ] Sufficiently addressed
> - [x] Some work remaining or caveats
> - [ ] Needs to be addressed
> 
> There is a readme, however the code is in need of proper documentation.

### Is there suitable collaboration infrastructure?

- Is the code in a version-controlled repository?
- Is there a license?
- Is an issue tracker used?
- Are there contribution guidelines?

> - [x] Sufficiently addressed
> - [ ] Some work remaining or caveats
> - [ ] Needs to be addressed
> 
> The code is stored in a GitHub, includes a license and uses github issues, for issue tracking.

### Are software dependencies listed and of appropriate quality?

> - [x] Sufficiently addressed
> - [ ] Some work remaining or caveats
> - [ ] Needs to be addressed
> 
> Dependencies are listed in the readme, with most being automatically included.

### Is input and output data handled carefully? (Models only)

- Does the code use the data pipeline for all inputs and outputs?
- Is the code appropriately parameterized (i.e. have hard coded parameters been removed)?

> - [ ] Sufficiently addressed
> - [ ] Some work remaining or caveats
> - [ ] Needs to be addressed
> - [x] N/A