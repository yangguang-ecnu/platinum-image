//////////////////////////////////////////////////////////////////////////
//
//  Listedfactory $Revision$
//
//  Object factory class used to keep track of interchangeable objects
//  of one particular class, like renderers or transfer functions
//
//  Adapted from an article by Robert Geiman
//  http://www.gamedev.net/reference/articles/article2097.asp
//
//  $LastChangedBy$

// This file is part of the Platinum library.
// Copyright (c) 2007 Uppsala University.
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

#include <FL/Fl_Menu.h>
#include <FL/Fl_Button.h>

#include <map>

typedef std::string factoryIdType;

//The syntax for using this final implementation of the object factory looks like this:
//
//ObjectFactory<Shape *(int), std::string> shape_factory;
//
//shape_factory.Register<Triangle>("triangle");
//shape_factory.Register<Square>("square");
//
//
//Shape *shape1 = shape_factory.Create("triangle", 10);
//Shape *shape2 = shape_factory.Create("square", 20);

template<typename BaseClassType, typename ClassType>
BaseClassType *CreateObject()
{
    return new ClassType;
}

template<typename BaseClassType>
class listedfactory
{
protected:
    typedef BaseClassType *(*CreateObjectFunc)();
    
protected:
    std::map<factoryIdType, CreateObjectFunc> m_object_creator;
    
public:
    typedef typename std::map<factoryIdType, CreateObjectFunc>::const_iterator ConstIterator;
    typedef typename std::map<factoryIdType, CreateObjectFunc>::iterator Iterator;
    
    template<typename ClassType>
        bool Register(factoryIdType unique_id)
        {
            if (m_object_creator.find(unique_id) != m_object_creator.end())
                return false;
            
            m_object_creator[unique_id] = &CreateObject<BaseClassType, ClassType>;
            
            return true;
        }
    
    bool Unregister(factoryIdType unique_id)
        {
        return (m_object_creator.erase(unique_id) == 1);
        }
    
    BaseClassType *Create(factoryIdType unique_id)
        {
        Iterator iter = m_object_creator.find(unique_id);
        
        if (iter == m_object_creator.end())
            return NULL;
        
        return ((*iter).second)();
        }
    
    ConstIterator begin() const
        {
            return m_object_creator.begin();
        }
    
    Iterator begin()
        {
        return m_object_creator.begin();
        }
    
    ConstIterator end() const
        {
            return m_object_creator.end();
        }
    
    Iterator end()
        {
        return m_object_creator.end();
        }
    
    //typename Iterator::SizeType num_items ()
    typename std::map<factoryIdType, CreateObjectFunc>::size_type num_items ()

        {
        return m_object_creator.size();
        }
    
    Fl_Menu_Item * function_menu (Fl_Callback * cb);
    
    //TODO: copy code from transferfactory::function_menu
    //      rename 'function_menu' to 'menu'
    
    void buttons (Fl_Group * box,bool horizontal,Fl_Callback * cb)
    {
        int buttonSize  = horizontal? box->h():box->w();
        int x = box->x();
        int y = box->y();
        
        box->begin();

        for (ConstIterator i = m_object_creator.begin();i != m_object_creator.end();i++)
            {
            std::string name = i->first;
            Fl_Button * button = new Fl_Button (x,y,buttonSize,buttonSize);
            button->label(strdup(name.substr(0,1).c_str()));
            button->tooltip(strdup(name.c_str()));
            
            button->box(FL_UP_BOX);
            button->down_box(FL_DOWN_BOX);
            button->type(FL_RADIO_BUTTON);
            button->callback((cb));
            button->user_data(reinterpret_cast<const void*>(&(i->first)));
            
            if (horizontal)
                { x += buttonSize; }
            else
                { y += buttonSize; }
            }
        
        box->size(x,y);
        box->end();
    }
};
