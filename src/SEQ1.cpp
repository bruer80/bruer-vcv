#include "plugin.hpp"

struct SEQ1 : Module {
    enum ParamIds {
        GENERATE_PARAM,
        LENGTH_PARAM,
        SCALE_PARAM,
        CLEAR_PARAM,

        EUCL1_PARAM,
        EUCL2_PARAM,
        EUCL3_PARAM,
        
        NOTE_C_PARAM,
        NOTE_CS_PARAM,
        NOTE_D_PARAM,
        NOTE_DS_PARAM,
        NOTE_E_PARAM,
        NOTE_F_PARAM,
        NOTE_FS_PARAM,
        NOTE_G_PARAM,
        NOTE_GS_PARAM,
        NOTE_A_PARAM,
        NOTE_AS_PARAM,
        NOTE_B_PARAM,

        NUM_PARAMS
    };

    enum InputIds {
        CLOCK_INPUT,
        RESET_INPUT,

        NUM_INPUTS
    };

    enum OutputIds {
        CV_OUTPUT,

        TRIG1_OUTPUT,
        TRIG2_OUTPUT,
        TRIG3_OUTPUT,

        NUM_OUTPUTS
    };

    enum LightIds {
        CLOCK_LIGHT,

        BIT1_LIGHT,
        BIT2_LIGHT,
        BIT3_LIGHT,
        BIT4_LIGHT,

        BIT5_LIGHT,
        BIT6_LIGHT,
        BIT7_LIGHT,
        BIT8_LIGHT,

        BIT9_LIGHT,
        BIT10_LIGHT,
        BIT11_LIGHT,
        BIT12_LIGHT,

        BIT13_LIGHT,
        BIT14_LIGHT,
        BIT15_LIGHT,
        BIT16_LIGHT,
        
        EUCL1_1_LIGHT,
        EUCL1_2_LIGHT,
        EUCL1_3_LIGHT,
        EUCL1_4_LIGHT,
        EUCL1_5_LIGHT,
        EUCL1_6_LIGHT,
        EUCL1_7_LIGHT,
        EUCL1_8_LIGHT,
        EUCL1_9_LIGHT,
        EUCL1_10_LIGHT,
        EUCL1_11_LIGHT,
        EUCL1_12_LIGHT,
        EUCL1_13_LIGHT,
        EUCL1_14_LIGHT,
        EUCL1_15_LIGHT,
        EUCL1_16_LIGHT,

        EUCL2_1_LIGHT,
        EUCL2_2_LIGHT,
        EUCL2_3_LIGHT,
        EUCL2_4_LIGHT,
        EUCL2_5_LIGHT,
        EUCL2_6_LIGHT,
        EUCL2_7_LIGHT,
        EUCL2_8_LIGHT,
        EUCL2_9_LIGHT,
        EUCL2_10_LIGHT,
        EUCL2_11_LIGHT,
        EUCL2_12_LIGHT,
        EUCL2_13_LIGHT,
        EUCL2_14_LIGHT,
        EUCL2_15_LIGHT,
        EUCL2_16_LIGHT,

        EUCL3_1_LIGHT,
        EUCL3_2_LIGHT,
        EUCL3_3_LIGHT,
        EUCL3_4_LIGHT,
        EUCL3_5_LIGHT,
        EUCL3_6_LIGHT,
        EUCL3_7_LIGHT,
        EUCL3_8_LIGHT,
        EUCL3_9_LIGHT,
        EUCL3_10_LIGHT,
        EUCL3_11_LIGHT,
        EUCL3_12_LIGHT,
        EUCL3_13_LIGHT,
        EUCL3_14_LIGHT,
        EUCL3_15_LIGHT,
        EUCL3_16_LIGHT,
        
        NOTE_C_LIGHT,
        NOTE_CS_LIGHT,
        NOTE_D_LIGHT,
        NOTE_DS_LIGHT,
        NOTE_E_LIGHT,
        NOTE_F_LIGHT,
        NOTE_FS_LIGHT,
        NOTE_G_LIGHT,
        NOTE_GS_LIGHT,
        NOTE_A_LIGHT,
        NOTE_AS_LIGHT,
        NOTE_B_LIGHT,

        NUM_LIGHTS
    };

    int step = 0;
    int displayStep = 0;
    bool shiftRegister[16] = {
        false,false,false,false,
        false,false,false,false,
        false,false,false,false,
        false,false,false,false
    };
    dsp::SchmittTrigger clockTrigger;
    dsp::SchmittTrigger resetTrigger;
    dsp::SchmittTrigger clearTrigger;
    
    bool noteEnabled[12] = {
        true, false, true, false,
        true, true, false,
        true, false,
        true, false,
        true
        
    };
    
    int ranges[24];

    dsp::SchmittTrigger noteTriggers[12];
    
    dsp::PulseGenerator trig1Pulse;
    dsp::PulseGenerator trig2Pulse;
    dsp::PulseGenerator trig3Pulse;
    
    int shift1 = 0;
    int shift2 = 0;
    int shift3 = 0;

    float lightTimer = 0.f;

    bool euclid(int step, int hits, int length) {
        if (hits <= 0)
            return false;

        if (hits >= length)
            return true;

        return ((step * hits) % length) < hits;
    }

    SEQ1() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configInput(CLOCK_INPUT, "Clock");
        configInput(RESET_INPUT, "Reset");
        configParam(CLEAR_PARAM, 0.f, 1.f, 0.f, "Clear");

        configOutput(CV_OUTPUT, "CV");

        configOutput(TRIG1_OUTPUT, "Trigger 1");
        configOutput(TRIG2_OUTPUT, "Trigger 2");
        configOutput(TRIG3_OUTPUT, "Trigger 3");

        configParam(GENERATE_PARAM, 0.f, 1.f, 0.f, "Generate");
        configParam(LENGTH_PARAM, 1.f, 16.f, 8.f, "Length");
        paramQuantities[LENGTH_PARAM]->snapEnabled = true;
        configParam(SCALE_PARAM, 1.f, 8.f, 2.f, "Scale");
        paramQuantities[SCALE_PARAM]->snapEnabled = true;

        configParam(EUCL1_PARAM, 0.f, 16.f, 4.f, "Euclidean 1");
        configParam(EUCL2_PARAM, 0.f, 16.f, 5.f, "Euclidean 2");
        configParam(EUCL3_PARAM, 0.f, 16.f, 7.f, "Euclidean 3");
        paramQuantities[EUCL1_PARAM]->snapEnabled = true;
        paramQuantities[EUCL2_PARAM]->snapEnabled = true;
        paramQuantities[EUCL3_PARAM]->snapEnabled = true;

        configParam(NOTE_C_PARAM, 0.f, 1.f, 1.f, "C");
        configParam(NOTE_CS_PARAM, 0.f, 1.f, 0.f, "C#");
        configParam(NOTE_D_PARAM, 0.f, 1.f, 1.f, "D");
        configParam(NOTE_DS_PARAM, 0.f, 1.f, 0.f, "D#");
        configParam(NOTE_E_PARAM, 0.f, 1.f, 1.f, "E");
        configParam(NOTE_F_PARAM, 0.f, 1.f, 1.f, "F");
        configParam(NOTE_FS_PARAM, 0.f, 1.f, 0.f, "F#");
        configParam(NOTE_G_PARAM, 0.f, 1.f, 1.f, "G");
        configParam(NOTE_GS_PARAM, 0.f, 1.f, 0.f, "G#");
        configParam(NOTE_A_PARAM, 0.f, 1.f, 1.f, "A");
        configParam(NOTE_AS_PARAM, 0.f, 1.f, 0.f, "A#");
        configParam(NOTE_B_PARAM, 0.f, 1.f, 1.f, "B");

        updateRanges();
    }
    
    void updateRanges() {

        bool anyEnabled = false;

        for (int note = 0; note < 12; note++) {
            if (noteEnabled[note]) {
                anyEnabled = true;
                break;
            }
        }

        for (int i = 0; i < 24; i++) {

            int closestNote = 0;
            int closestDist = 999;

            for (int note = -12; note <= 24; note++) {

                int dist =
                    abs((i + 1) / 2 - note);

                if (anyEnabled &&
                    !noteEnabled[(note % 12 + 12) % 12]) {
                    continue;
                }

                if (dist < closestDist) {
                    closestDist = dist;
                    closestNote = note;
                }
                else {
                    break;
                }
            }

            ranges[i] = closestNote;
        }
    }

    void process(const ProcessArgs& args) override {

        
        lightTimer -= args.sampleTime;
        if (lightTimer < 0.f)
            lightTimer = 0.f;

        lights[CLOCK_LIGHT].setBrightness(lightTimer > 0.f ? 1.f : 0.f);

        int length = (int)params[LENGTH_PARAM].getValue();

        for (int i = 0; i < length; i++) {

            lights[BIT1_LIGHT + i].setBrightness(
                shiftRegister[i] ? 1.f : 0.1f
            );
        }
        
        for (int i = length; i < 16; i++) {
            lights[BIT1_LIGHT + i].setBrightness(0.f);
        }

        lights[BIT1_LIGHT + step].setBrightness(3.f);
        
        if (resetTrigger.process(inputs[RESET_INPUT].getVoltage())) {
            step = 0;
        }
        
        if (clearTrigger.process(
            params[CLEAR_PARAM].getValue())) {

            for (int i = 0; i < 16; i++) {
                shiftRegister[i] = false;
            }

            step = 0;
        }

        if (clockTrigger.process(inputs[CLOCK_INPUT].getVoltage())) {

            lightTimer = 0.05f;

            int hits1 = (int)params[EUCL1_PARAM].getValue();
            int hits2 = (int)params[EUCL2_PARAM].getValue();
            int hits3 = (int)params[EUCL3_PARAM].getValue();

            float control =
                params[GENERATE_PARAM].getValue();
            
            int probCtrl =
                int(control * 10.f);

            int probCtrlRnd =
                int(random::uniform() * 10.f);

            bool incomingBit;

            if (probCtrlRnd < probCtrl) {
                incomingBit =
                    random::uniform() > 0.5f;
            }
            else {
                incomingBit =
                    shiftRegister[length - 1];
            }

            for (int i = 15; i > 0; i--) {
                shiftRegister[i] =
                    shiftRegister[i - 1];
            }

            shiftRegister[0] =
                incomingBit;
            
            int pos1 = (step + shift1) % length;
            int pos2 = (step + shift2) % length;
            int pos3 = (step + shift3) % length;

            if (euclid(pos1, hits1, length))
                trig1Pulse.trigger(0.005f);

            if (euclid(pos2, hits2, length))
                trig2Pulse.trigger(0.005f);

            if (euclid(pos3, hits3, length))
                trig3Pulse.trigger(0.005f);
            
            step++;

            if (step >= length)
                step = 0;
        }
        
        int hits1 = (int)params[EUCL1_PARAM].getValue();
        int hits2 = (int)params[EUCL2_PARAM].getValue();
        int hits3 = (int)params[EUCL3_PARAM].getValue();

        for (int i = 0; i < 16; i++) {
            lights[EUCL1_1_LIGHT + i].setBrightness(0.f);
            lights[EUCL2_1_LIGHT + i].setBrightness(0.f);
            lights[EUCL3_1_LIGHT + i].setBrightness(0.f);
        }

        for (int i = 0; i < 16; i++) {
            lights[EUCL1_1_LIGHT + i].setBrightness(0.f);
            lights[EUCL2_1_LIGHT + i].setBrightness(0.f);
            lights[EUCL3_1_LIGHT + i].setBrightness(0.f);
        }

        for (int i = 0; i < length; i++) {

            int p1 = (i + shift1) % length;
            int p2 = (i + shift2) % length;
            int p3 = (i + shift3) % length;

            if (euclid(i, hits1, length))
                lights[EUCL1_1_LIGHT + p1].setBrightness(1.f);

            if (euclid(i, hits2, length))
                lights[EUCL2_1_LIGHT + p2].setBrightness(1.f);

            if (euclid(i, hits3, length))
                lights[EUCL3_1_LIGHT + p3].setBrightness(1.f);
        }
        outputs[TRIG1_OUTPUT].setVoltage(
            trig1Pulse.process(args.sampleTime) ? 10.f : 0.f
        );

        outputs[TRIG2_OUTPUT].setVoltage(
            trig2Pulse.process(args.sampleTime) ? 10.f : 0.f
        );

        outputs[TRIG3_OUTPUT].setVoltage(
            trig3Pulse.process(args.sampleTime) ? 10.f : 0.f
        );

        float scale[12];
        int noteCount = 0;

        float semitones[12] = {
            0.f,   // C
            1.f,   // C#
            2.f,   // D
            3.f,   // D#
            4.f,   // E
            5.f,   // F
            6.f,   // F#
            7.f,   // G
            8.f,   // G#
            9.f,   // A
            10.f,  // A#
            11.f   // B
        };

        for (int i = 0; i < 12; i++) {
            if (noteEnabled[i]) {
                scale[noteCount++] = semitones[i] / 12.f;
            }
        }
        
        if (noteCount == 0) {
            scale[0] = 0.f;
            noteCount = 1;
        }
        
        for (int i = 0; i < 12; i++) {
            
            if (noteTriggers[i].process(
                params[NOTE_C_PARAM + i].getValue())) {

                    noteEnabled[i] = !noteEnabled[i];
                    updateRanges();
            }

            lights[NOTE_C_LIGHT + i].setBrightness(
                noteEnabled[i] ? 1.f : 0.f
            );
        }
        
        int value = 0;

        for (int i = 0; i < length; i++) {
            if (shiftRegister[i])
                value += (1 << i);
        }

        int octaveRange =
            (int)params[SCALE_PARAM].getValue();

        float normalized =
            (float)value /
            ((1 << length) - 1);

        int totalNotes =
            octaveRange * 12;

        int rawNote =
            normalized * (totalNotes - 1);

        int octave =
            rawNote / 12;

        int semitone =
            rawNote % 12;

        int note =
            ranges[semitone * 2] + octave * 12;

        float cv =
            note / 12.f;

        outputs[CV_OUTPUT].setVoltage(cv);

    }
};

struct SEQ1Widget : ModuleWidget {
    SEQ1Widget(SEQ1* module) {
        setModule(module);

        box.size = Vec(40, 380);

        setPanel(createPanel(asset::plugin(pluginInstance, "res/SEQ1.svg")));
        
        addParam(createParamCentered<TL1105>(
            mm2px(Vec(30, 14)),
            module,
            SEQ1::CLEAR_PARAM
        ));
        
        // Riga superiore
        for (int i = 0; i < 8; i++) {
            addChild(createLightCentered<SmallLight<RedLight>>(
                mm2px(Vec(8.0 + i * (34.0 / 7.0), 65)),
                module,
                SEQ1::BIT1_LIGHT + i
            ));
        }

        // Riga inferiore
        for (int i = 0; i < 8; i++) {
            addChild(createLightCentered<SmallLight<RedLight>>(
                mm2px(Vec(8.0 + i * (34.0 / 7.0), 75.5)),
                module,
                SEQ1::BIT9_LIGHT + i
            ));
        }
        
        for (int i = 0; i < 8; i++) {
            addChild(createLightCentered<TinyLight<GreenLight>>(
                mm2px(Vec(8.0 + i * (34.0 / 7.0), 67.5)),
                module,
                SEQ1::EUCL1_1_LIGHT + i
            ));

            addChild(createLightCentered<TinyLight<GreenLight>>(
                mm2px(Vec(8.0 + i * (34.0 / 7.0), 78)),
                module,
                SEQ1::EUCL1_9_LIGHT + i
            ));
        }
        
        for (int i = 0; i < 8; i++) {
            addChild(createLightCentered<TinyLight<YellowLight>>(
                mm2px(Vec(8.0 + i * (34.0 / 7.0), 70)),
                module,
                SEQ1::EUCL2_1_LIGHT + i
            ));

            addChild(createLightCentered<TinyLight<YellowLight>>(
                mm2px(Vec(8.0 + i * (34.0 / 7.0), 80.5)),
                module,
                SEQ1::EUCL2_9_LIGHT + i
            ));
        }
        
        for (int i = 0; i < 8; i++) {
            addChild(createLightCentered<TinyLight<BlueLight>>(
                mm2px(Vec(8.0 + i * (34.0 / 7.0), 72.5)),
                module,
                SEQ1::EUCL3_1_LIGHT + i
            ));

            addChild(createLightCentered<TinyLight<BlueLight>>(
                mm2px(Vec(8.0 + i * (34.0 / 7.0), 83)),
                module,
                SEQ1::EUCL3_9_LIGHT + i
            ));
        }
        
        // C
        addParam(createParamCentered<TL1105>(
            mm2px(Vec(50, 15)),
            module,
            SEQ1::NOTE_C_PARAM
        ));
        addChild(createLightCentered<MediumLight<WhiteLight>>(
            mm2px(Vec(56, 15)),
            module,
            SEQ1::NOTE_C_LIGHT
        ));

        // C#
        addParam(createParamCentered<TL1105>(
            mm2px(Vec(50, 24)),
            module,
            SEQ1::NOTE_CS_PARAM
        ));
        addChild(createLightCentered<SmallLight<WhiteLight>>(
            mm2px(Vec(56, 24)),
            module,
            SEQ1::NOTE_CS_LIGHT
        ));

        // D
        addParam(createParamCentered<TL1105>(
            mm2px(Vec(50, 33)),
            module,
            SEQ1::NOTE_D_PARAM
        ));
        addChild(createLightCentered<MediumLight<WhiteLight>>(
            mm2px(Vec(56, 33)),
            module,
            SEQ1::NOTE_D_LIGHT
        ));

        // D#
        addParam(createParamCentered<TL1105>(
            mm2px(Vec(50, 42)),
            module,
            SEQ1::NOTE_DS_PARAM
        ));
        addChild(createLightCentered<SmallLight<WhiteLight>>(
            mm2px(Vec(56, 42)),
            module,
            SEQ1::NOTE_DS_LIGHT
        ));

        // E
        addParam(createParamCentered<TL1105>(
            mm2px(Vec(50, 51)),
            module,
            SEQ1::NOTE_E_PARAM
        ));
        addChild(createLightCentered<MediumLight<WhiteLight>>(
            mm2px(Vec(56, 51)),
            module,
            SEQ1::NOTE_E_LIGHT
        ));

        // F
        addParam(createParamCentered<TL1105>(
            mm2px(Vec(50, 60)),
            module,
            SEQ1::NOTE_F_PARAM
        ));
        addChild(createLightCentered<MediumLight<WhiteLight>>(
            mm2px(Vec(56, 60)),
            module,
            SEQ1::NOTE_F_LIGHT
        ));

        // F#
        addParam(createParamCentered<TL1105>(
            mm2px(Vec(50, 69)),
            module,
            SEQ1::NOTE_FS_PARAM
        ));
        addChild(createLightCentered<SmallLight<WhiteLight>>(
            mm2px(Vec(56, 69)),
            module,
            SEQ1::NOTE_FS_LIGHT
        ));

        // G
        addParam(createParamCentered<TL1105>(
            mm2px(Vec(50, 78)),
            module,
            SEQ1::NOTE_G_PARAM
        ));
        addChild(createLightCentered<MediumLight<WhiteLight>>(
            mm2px(Vec(56, 78)),
            module,
            SEQ1::NOTE_G_LIGHT
        ));

        // G#
        addParam(createParamCentered<TL1105>(
            mm2px(Vec(50, 87)),
            module,
            SEQ1::NOTE_GS_PARAM
        ));
        addChild(createLightCentered<SmallLight<WhiteLight>>(
            mm2px(Vec(56, 87)),
            module,
            SEQ1::NOTE_GS_LIGHT
        ));

        // A
        addParam(createParamCentered<TL1105>(
            mm2px(Vec(50, 96)),
            module,
            SEQ1::NOTE_A_PARAM
        ));
        addChild(createLightCentered<MediumLight<WhiteLight>>(
            mm2px(Vec(56, 96)),
            module,
            SEQ1::NOTE_A_LIGHT
        ));

        // A#
        addParam(createParamCentered<TL1105>(
            mm2px(Vec(50, 105)),
            module,
            SEQ1::NOTE_AS_PARAM
        ));
        addChild(createLightCentered<SmallLight<WhiteLight>>(
            mm2px(Vec(56, 105)),
            module,
            SEQ1::NOTE_AS_LIGHT
        ));

        // B
        addParam(createParamCentered<TL1105>(
            mm2px(Vec(50, 114)),
            module,
            SEQ1::NOTE_B_PARAM
        ));
        addChild(createLightCentered<MediumLight<WhiteLight>>(
            mm2px(Vec(56, 114)),
            module,
            SEQ1::NOTE_B_LIGHT
        ));
        
        addParam(createParamCentered<LEDSliderGreen>(
            mm2px(Vec(14, 42)),
            module,
            SEQ1::GENERATE_PARAM
        ));
        
        addParam(createParamCentered<RoundLargeBlackKnob>(
            mm2px(Vec(33, 30)),
            module,
            SEQ1::SCALE_PARAM
        ));
        
        addParam(createParamCentered<RoundLargeBlackKnob>(
            mm2px(Vec(33, 50)),
            module,
            SEQ1::LENGTH_PARAM
        ));
        
        addParam(createParamCentered<RoundLargeBlackKnob>(
            mm2px(Vec(11, 94)),
            module,
            SEQ1::EUCL1_PARAM
        ));
        
        addParam(createParamCentered<RoundLargeBlackKnob>(
            mm2px(Vec(25, 94)),
            module,
            SEQ1::EUCL2_PARAM
        ));
        
        addParam(createParamCentered<RoundLargeBlackKnob>(
            mm2px(Vec(39, 94)),
            module,
            SEQ1::EUCL3_PARAM
        ));
        
        addInput(createInputCentered<PJ301MPort>(
            mm2px(Vec(9, 14)),
            module,
            SEQ1::CLOCK_INPUT
        ));

        addInput(createInputCentered<PJ301MPort>(
            mm2px(Vec(20, 14)),
            module,
            SEQ1::RESET_INPUT
        ));

        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(9.5, 110)),
            module,
            SEQ1::TRIG1_OUTPUT
        ));

        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(18.5, 110)),
            module,
            SEQ1::TRIG2_OUTPUT
        ));

        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(27.5, 110)),
            module,
            SEQ1::TRIG3_OUTPUT
        ));

        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(40, 110)),
            module,
            SEQ1::CV_OUTPUT
        ));
        
    }
    
    void appendContextMenu(Menu* menu) override {
        SEQ1* module = dynamic_cast<SEQ1*>(this->module);

        menu->addChild(new MenuSeparator());

        struct ShiftItem : MenuItem {
            int* value;
            int shift;

            void onAction(const event::Action& e) override {
                *value = shift;
            }
        };

        menu->addChild(createSubmenuItem(
            "Euclid 1 Shift",
            std::to_string(module->shift1),
            [=](Menu* menu) {
                for (int i = 0; i < 16; i++) {
                    ShiftItem* item = createMenuItem<ShiftItem>(
                        std::to_string(i)
                    );
                    item->rightText =
                        CHECKMARK(module->shift1 == i);
                    item->value = &module->shift1;
                    item->shift = i;
                    menu->addChild(item);
                }
            }
        ));

        menu->addChild(createSubmenuItem(
            "Euclid 2 Shift",
            std::to_string(module->shift2),
            [=](Menu* menu) {
                for (int i = 0; i < 16; i++) {
                    ShiftItem* item = createMenuItem<ShiftItem>(
                        std::to_string(i)
                    );
                    item->rightText =
                        CHECKMARK(module->shift2 == i);
                    item->value = &module->shift2;
                    item->shift = i;
                    menu->addChild(item);
                }
            }
        ));

        menu->addChild(createSubmenuItem(
            "Euclid 3 Shift",
            std::to_string(module->shift3),
            [=](Menu* menu) {
                for (int i = 0; i < 16; i++) {
                    ShiftItem* item = createMenuItem<ShiftItem>(
                        std::to_string(i)
                    );
                    item->rightText =
                        CHECKMARK(module->shift3 == i);
                    item->value = &module->shift3;
                    item->shift = i;
                    menu->addChild(item);
                }
            }
        ));
    }
};

Model* modelSEQ1 = createModel<SEQ1, SEQ1Widget>("SEQ1");
