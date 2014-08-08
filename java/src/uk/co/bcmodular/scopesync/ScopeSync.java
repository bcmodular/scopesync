/**
 * ScopeSync mxj~ Main class
 *
 *  (C) Copyright 2014 bcmodular (http://www.bcmodular.co.uk/)
 *
 * This file is part of ScopeSync.
 *
 * ScopeSync is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * ScopeSync is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ScopeSync.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contributors:
 *  Simon Russell
 *  Will Ellis
 *  Jessica Brandt
 */
package uk.co.bcmodular.scopesync;

import java.util.Arrays;

import com.cycling74.max.Atom;
import com.cycling74.max.DataTypes;
import com.cycling74.msp.MSPPerformer;
import com.cycling74.msp.MSPSignal;

public class ScopeSync extends MSPPerformer
{

    // Debug mode setting
    private static final boolean DEBUG_MODE = false;

    // Array of Controls being tracked
    private Control[] controls;

    // Number of controls being tracked
    private static final int NUM_CONTROLS = 128;

    // Array position of the last Control whose change was sent
    private int lastSigvControlIdx;

    // Debug poster
    private void debugPost(String s)
    {
        if (DEBUG_MODE)
        {
            post(s);
        }
    }

    // Constructor
    public ScopeSync(Atom[] args)
    {
        declareInlets(new int[]      { SIGNAL, SIGNAL, DataTypes.ALL });
        declareOutlets(new int[]     { SIGNAL, SIGNAL, DataTypes.ALL });
        setInletAssist(new String[]  { "ScopeSync Data In", "ScopeSync Dest In", "Incoming Messages" });
        setOutletAssist(new String[] { "ScopeSync Data Out", "ScopeSync Dest Out", "Outgoing Messages" });
        createInfoOutlet(false);

        setupControls();
    }

    // Create Destinations if arguments have been supplied
    private void setupControls()
    {
        lastSigvControlIdx = NUM_CONTROLS - 1;

        controls = new Control[NUM_CONTROLS];

       // Add control dests to array
        for (int i = 0; i < NUM_CONTROLS; i++)
        {
            debugPost("Adding control dest to array: " + i);
            controls[i] = new Control(i);
        }

    }

    // Handle incoming lists
    @Override
    public void list(Atom[] listInput)
    {
        int inletNum = getInlet();
        int listLength = listInput.length;

        post("********************************************");
        post("Invalid input to ScopeSync, inlet: " + inletNum);
        Atom a;

        for (int i = 0; i < listLength; i++)
        {
            a = listInput[i];
            post("List element " + i + " has value" + a.getInt());
        }
        post("********************************************");

    }

    // Handle incoming Messages (expected to be Control Data or Snapshot
    // messages)
    @Override
    public void anything(String msg, Atom[] controlData)
    {
        int     inletNum     = getInlet();
        int     listLength   = controlData.length;
        boolean invalidInput = true;

        if (listLength == 0)
        {
            if (msg.equals("snapshot"))
            {
                for (int i = 0; i < NUM_CONTROLS; i++)
                {
                    controls[i].snapshot = true;
                }
                invalidInput = false;
            }
        }
        else if (listLength == 1)
        {
            // Process a Control Change
            float controlDataValue = controlData[0].getFloat();

            // Firstly look up the ctrlIdx given the supplied destCode
            int ctrlIdx = Arrays.asList(Control.CTRL_CODES).indexOf(msg);

            if (ctrlIdx != -1)
            {
                // Then grab that element from the control destination array
                Control control = controls[ctrlIdx];
                control.setValue(controlDataValue, true, false);
                invalidInput = false;
            }
        }

        if (invalidInput)
        {
            post("********************************************");
            post("Invalid message input to ScopeSync, message: " + msg
                    + ", inlet: " + inletNum);
            post("List length: " + listLength);
            Atom a;
            for (int i = 0; i < listLength; i++)
            {
                a = controlData[i];
                post("List element " + i + " has value" + a.getInt());
            }
            post("********************************************");
        }
    }

    // Process next signal vector
    @Override
    public void perform(MSPSignal[] in, MSPSignal[] out)
    {
        float[] in1 = in[0].vec; // Input signal vector for the Data
        float[] in2 = in[1].vec; // Input signal vector for the Destination

        for (int i = 0; i < in1.length; i++)
        {
            // Read current sample values to see if there are any control updates
            // to add to the queue
            if (in2[i] > 0.0f)
            {
                int index = Control.getControlIdxFromDestValue(in2[i]);

                if (index != -1)
                {
                    Control control = controls[index];

                    if (control.deadTimeCount == 0)
                    {
                        control.setValue(in1[i], false, true);

                        // Send an update to the 1st outlet
                        outlet(0, new Atom[]{Atom.newAtom(control.ctrlCode), Atom.newAtom("rawfloat"), Atom.newAtom(in1[i])});
                    }
                }
            }
        }

        float[] out1 = out[0].vec; // Output signal vector for the Data
        float[] out2 = out[1].vec; // Output signal vector for the Destination

        // Blank the output vectors
        for (int i = 0; i < out1.length; i++)
        {
            out1[i] = 0.0f;
            out2[i] = 0.0f;
        }

        // Now fill in gaps with any control changes that have happened
        int ctrlIdx = -1;

        // Loop through each of the signal vector items
        for (int i = 0; i < out1.length; i++)
        {
            // Loop through the Controls, starting at the one after the
            // last one we looked at
            while (ctrlIdx != lastSigvControlIdx)
            {
                if (ctrlIdx == -1)
                {
                    // We've entered for the first time, so start with
                    // the next one after the last one reached in the
                    // previous signal vector
                    ctrlIdx = (lastSigvControlIdx + 1) % NUM_CONTROLS;
                }
                else
                {
                    // Increment, but need to loop around the Controls
                    ctrlIdx = (ctrlIdx + 1) % NUM_CONTROLS;
                }

                Control control = controls[ctrlIdx];

                if (control.dataChanged())
                {
                    // The value has changed since we last sent one for
                    // this Control, so we should send an update
                    out1[i] = control.dataVal;
                    out2[i] = control.destVal;

                    control.markDataSent();
                    break;
                }
            }

            if (ctrlIdx == lastSigvControlIdx)
            {
                break;
            }
        }

        lastSigvControlIdx = ctrlIdx;

        for (int i = 0; i < NUM_CONTROLS; i++)
        {
            controls[i].decDeadTimeCount();
        }
    }
}
