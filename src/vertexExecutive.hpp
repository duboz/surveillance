/** 
 * @file vertexExecutive.hpp
 * @brief VertexExecutive class
 * @author Bruno Bonte
 * @date mardi 9 février 2010, 11:40:50 (UTC+0100)
 */

/*
 * Copyright (C) 2010 - Bruno Bonte
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef MODEL_VERTEX_EXECUTIVE_HPP
#define MODEL_VERTEX_EXECUTIVE_HPP

#include <vle/devs/Executive.hpp>

namespace model {

/*
*@brief VertexExecutive class defines the dynamic of the executive devs model
* present in each node. At initialisation it connects models that are inside its node
* to output and input port of its node. 
*/
    class VertexExecutive : public vle::devs::Executive
    {

    public:
        virtual ~VertexExecutive()
        {}

        VertexExecutive(const vle::devs::ExecutiveInit &,
                         const vle::devs::InitEventList &);

        virtual vle::devs::Time init(const vle::devs::Time &);
        void internalTransition(const vle::devs::Time &);
        vle::devs::Time timeAdvance() const;
    private:
        vle::value::Set m_active_collectors;
        vle::value::Set m_passive_collectors;
        enum PHASE {INIT,INITIALIZED};
        PHASE m_phase;
    };
}
#endif
