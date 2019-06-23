import React, {Component} from 'react';
import {Link} from "react-router-dom";

import {ROUTE_PREFIX, getUser} from '../const.js'

class UserInfo extends Component {
    constructor(props) {
        super(props)
        this.state = {
            user: {}
        }
    }

    componentDidMount() {
        this.setState({
            user: getUser()
        })
    }

    render() {
        var user = this.state.user
        return (
            <div>
                <nav className="subHeader__nav--small">
                    <div className="layout__inner">
                        <Link to={`${ROUTE_PREFIX}`}>Zpátky do obchodu</Link>
                    </div>
                </nav>
                <div className="admin admin__start layout__inner">

       
                    <div className="admin__start__text">
                        <h1>Váš účet</h1>
                        <small>Zde najdete informace o vašem účtu, navíc se zde můžete odlhásit.</small>
                    </div>

                    <div className="admin__userInfo">
                        <p>{user.name}</p>
                        <p>{user.email}</p>
                        <p>{user.address}</p>
                        <p>{user.level === 1 ? 'Maloobchodník' : 'Velkoobchodník'}</p>
                        <Link to={`${ROUTE_PREFIX}logout`} className="button">Odhlásit se</Link>
                    </div>
                </div>
            </div>
        )
    }
}

export default UserInfo