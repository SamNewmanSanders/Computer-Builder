This project was inspired by the series https://www.youtube.com/watch?v=QZwneRb-zqA where parts of a computer were built directly from logic gates.

I wanted to do a similar thing without looking at the source code (promise), and I did the Visuals with SFML where I think he may have used Unity and C#?

At first I tried a pointer based approach where signals just point to their next destination. It worked fine for one level but as soon as you try and package up a circuit into 
a component you have to copy pointers etc etc which was difficult. Learnt more about pointers / deep copying though.

So then I simply went for an approach where each circuit is just described by vectors and the definition (components, connections etc) is kept seperate from the state (values of wires / pins)
This made it all easy to package and to copy aswell into Json files so information can be retained between program runs.
I tried for a kind of Model-Visualiser-Controller setup to organize the code which worked fairly well. Would like to learn about alternative ECS systems more in the future.

Once the program worked building all the components only took a couple hours.
Highest level component I made was an ALU with registers thing (?) that used a multiplexer to choose between user inputs, the adder and can 
increment numbers, add etc.

**SCREENSHOTS OF THE LAYOUTS CAN BE FOUND IN THE SCREENSHOTS FOLDER**

**Here are some videos of incrementing by 1 and 3**
https://drive.google.com/file/d/1mBGfP4qFJe_L-1mFtp2qEk5Ad4HuKTa8/view?usp=drive_link
https://drive.google.com/file/d/1_LpDiMXZiLFYp2hEke0kBGfsyNaPcyvi/view?usp=sharing


Still plenty of other things I would like to add (deleting placed things, clicking on a component and seeing its composition inside, **two's complement**, digital display) 
but I have other things to do tbh.

