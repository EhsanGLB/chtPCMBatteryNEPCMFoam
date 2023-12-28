    // Initialise solid field pointer lists
    PtrList<volScalarField> rhos(solidRegions.size());
    PtrList<volScalarField> cps(solidRegions.size());
    PtrList<volScalarField> rhosCps(solidRegions.size());
    PtrList<volScalarField> Kappas(solidRegions.size());
    PtrList<volScalarField> Ts(solidRegions.size());

    // Populate solid field pointer lists
    forAll(solidRegions, i)
    {
        Info<< "*** Reading solid mesh thermophysical properties for region "
            << solidRegions[i].name() << nl << endl;

        Info<< "    Adding to rhos\n" << endl;
        rhos.set
        (
            i,
            new volScalarField
            (
                IOobject
                (
                    "rho",
                    runTime.timeName(),
                    solidRegions[i],
                    IOobject::MUST_READ,
                    IOobject::AUTO_WRITE
                ),
                solidRegions[i]
            )
        );

        Info<< "    Adding to cps\n" << endl;
        cps.set
        (
            i,
            new volScalarField
            (
                IOobject
                (
                    "cp",
                    runTime.timeName(),
                    solidRegions[i],
                    IOobject::MUST_READ,
                    IOobject::AUTO_WRITE
                ),
                solidRegions[i]
            )
        );

        rhosCps.set
        (
            i,
            new volScalarField("rhosCps", rhos[i]*cps[i])
        );

        Info<< "    Adding to Kappas\n" << endl;
        Kappas.set
        (
            i,
            new volScalarField
            (
                IOobject
                (
                    "Kappa",
                    runTime.timeName(),
                    solidRegions[i],
                    IOobject::MUST_READ,
                    IOobject::AUTO_WRITE
                ),
                solidRegions[i]
            )
        );

        Info<< "    Adding to Ts\n" << endl;
        Ts.set
        (
            i,
            new volScalarField
            (
                IOobject
                (
                    "T",
                    runTime.timeName(),
                    solidRegions[i],
                    IOobject::MUST_READ,
                    IOobject::AUTO_WRITE
                ),
                solidRegions[i]
            )
        );
    }


// Battery
    PtrList<IOdictionary> batteryProperties(solidRegions.size());
    PtrList<word> stateBattery(solidRegions.size());
    PtrList<scalar> CBattery(solidRegions.size());
    PtrList<scalar> ISOCBattery(solidRegions.size());
    PtrList<dimensionedScalar> RBattery(solidRegions.size());
    PtrList<dimensionedScalar> IBattery(solidRegions.size());
    PtrList<dimensionedScalar> dEdTBattery(solidRegions.size());
    scalarList SOC(solidRegions.size());


    forAll(solidRegions, i)
    {
        Info<< "    Adding to batteryProperties\n" << endl;
        batteryProperties.set
        (
            i,
            new IOdictionary
            (
                IOobject
                (
                    "batteryProperties",
                    runTime.constant(),
                    solidRegions[i],
                    IOobject::READ_IF_PRESENT,
                    IOobject::NO_WRITE
                )
            )
        );

        if(batteryProperties[i].found("state"))
        {
            stateBattery.set
            (
                i,
                new word (batteryProperties[i].lookup("state"))
            );
        }

        if(batteryProperties[i].found("C"))
        {
            CBattery.set
            (
                i,
                new scalar (readScalar(batteryProperties[i].lookup("C")))
            );
        }

        if(batteryProperties[i].found("ISOC"))
        {
            ISOCBattery.set
            (
                i,
                new scalar (readScalar(batteryProperties[i].lookup("ISOC")))
            );

            SOC[i] = ISOCBattery[i];
        }

        if(batteryProperties[i].found("R"))
        {
            RBattery.set
            (
                i,
                new dimensionedScalar (batteryProperties[i].lookup("R"))
            );
        }

        if(batteryProperties[i].found("I"))
        {
            IBattery.set
            (
                i,
                new dimensionedScalar (batteryProperties[i].lookup("I"))
            );
        }

        if(batteryProperties[i].found("dEdT"))
        {
            dEdTBattery.set
            (
                i,
                new dimensionedScalar (batteryProperties[i].lookup("dEdT"))
            );
        }
    }

// Heat source
    PtrList<IOdictionary> heatSourceProperties(solidRegions.size());
    PtrList<dimensionedScalar> hsSolid(solidRegions.size());

    forAll(solidRegions, i)
    {
        Info<< "    Adding to heatSourceProperties\n" << endl;
        heatSourceProperties.set
        (
            i,
            new IOdictionary
            (
                IOobject
                (
                    "heatSourceProperties",
                    runTime.constant(),
                    solidRegions[i],
                    IOobject::READ_IF_PRESENT,
                    IOobject::NO_WRITE
                )
            )
        );

        if(heatSourceProperties[i].found("hs"))
        {
            hsSolid.set
            (
                i,
                new dimensionedScalar (heatSourceProperties[i].lookup("hs"))
            );
        }
        else
        {
            hsSolid.set
            (
                i,
                new dimensionedScalar ("hs", dimThermalConductivity*dimTemperature/dimArea, SMALL)
            );
        }
    }

    scalar tOld = 0.0;
